# SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
<#
.SYNOPSIS
    Dot-sourced by the CI runners of the API suites: issues the server token
    ProLife uses at Puma. Without it Puma refuses every request of ProLife.
#>

function Invoke-ProductGql([string]$ProductId, [int]$Port, [string]$Query, [string]$Token) {
    $headers = @{ productid = $ProductId }
    if ($Token) { $headers['x-authentication-token'] = $Token }
    $body = [Text.Encoding]::UTF8.GetBytes((@{ query = $Query } | ConvertTo-Json -Compress))
    return Invoke-RestMethod -Uri "http://localhost:$Port/$ProductId/graphql" -Method Post -Headers $headers -ContentType 'application/json; charset=utf-8' -Body $body
}

# A receiver issues its tokens to its superuser
function Get-SuperuserToken([string]$ProductId, [int]$Port) {
    $environmentFile = Join-Path $PSScriptRoot 'ProLifeApi-Dev.postman_environment.json'
    $suPassword = (Get-Content $environmentFile -Raw | ConvertFrom-Json).values | Where-Object { $_.key -eq 'suPassword' } | Select-Object -ExpandProperty value
    # A JSON string literal is a valid GraphQL one: quotes and backslashes in the password stay escaped
    $passwordLiteral = ConvertTo-Json ([string]$suPassword) -Compress
    $response = Invoke-ProductGql $ProductId $Port "query Authorization { Authorization(input: { login: `"su`", password: $passwordLiteral, productId: `"$ProductId`" }) { token } }" ''
    $suToken = $response.data.Authorization.token
    if (-not $suToken) { throw "Unable to log in to $ProductId to issue the server tokens" }
    return $suToken
}

# The token holds every permission the receiver offers the client: the suite exercises everything the products do
function New-ServerToken([string]$ReceiverProductId, [int]$Port, [string]$SuToken, [string]$ClientProductId) {
    $response = Invoke-ProductGql $ReceiverProductId $Port "query GetServerTokenReceiver { GetServerTokenReceiver(input: { clientProductId: `"$ClientProductId`" }) { groups { entries { permissionId } } } }" $SuToken
    $scopeList = (@($response.data.GetServerTokenReceiver.groups | ForEach-Object { $_.entries } | ForEach-Object { "`"$($_.permissionId)`"" })) -join ', '

    $response = Invoke-ProductGql $ReceiverProductId $Port "mutation CreateServerToken { CreateServerToken(input: { clientProductId: `"$ClientProductId`", name: `"$ClientProductId for $ReceiverProductId`", scopes: [$scopeList] }) { token success } }" $SuToken
    if (-not $response.data.CreateServerToken.success) {
        throw "Unable to issue the token of $ClientProductId for ${ReceiverProductId}: $($response.errors | ConvertTo-Json -Depth 4 -Compress)"
    }
    return $response.data.CreateServerToken.token
}

# A receiver issues tokens to its clients only: the configurator (Agentino) makes the product one first
function Add-ServerTokenClient([string]$ReceiverProductId, [int]$Port, [string]$SuToken, [string]$ClientProductId) {
    $response = Invoke-ProductGql $ReceiverProductId $Port "mutation AddServerTokenClient { AddServerTokenClient(input: { clientProductId: `"$ClientProductId`", name: `"$ClientProductId`" }) { success } }" $SuToken
    if (-not $response.data.AddServerTokenClient.success) {
        throw "Unable to make $ClientProductId a client of ${ReceiverProductId}: $($response.errors | ConvertTo-Json -Depth 4 -Compress)"
    }
}

# The servers watch each other's collections with tokens of their own: without
# them the subscriptions are refused and the caches of a product get stale.
function New-PumaServerTokens([int]$PumaPort) {
    Write-Host "`n=== Issuing the tokens of the product servers for Puma ===" -ForegroundColor Cyan
    $suToken = Get-SuperuserToken 'Puma' $PumaPort

    Add-ServerTokenClient 'Puma' $PumaPort $suToken 'ProLife'

    return @{
        ProLifeForPuma = New-ServerToken 'Puma' $PumaPort $suToken 'ProLife'
    }
}
