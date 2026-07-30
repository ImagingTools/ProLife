-- Migration: Online License Activation tables
-- Adds support for online license activation via activation keys

CREATE TABLE IF NOT EXISTS "ActivationKeys" (
    "Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "ActivationKey" VARCHAR(64) NOT NULL UNIQUE,
    "ProductId" UUID NOT NULL,
    "LicenseId" UUID NOT NULL,
    "MaxActivations" INTEGER NOT NULL DEFAULT 2,
    "IsActive" BOOLEAN NOT NULL DEFAULT TRUE,
    "CreatedAt" TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "ExpiresAt" TIMESTAMP WITH TIME ZONE,
    "CreatedBy" UUID
);

CREATE INDEX idx_activation_keys_key ON "ActivationKeys" ("ActivationKey");
CREATE INDEX idx_activation_keys_product ON "ActivationKeys" ("ProductId");

CREATE TABLE IF NOT EXISTS "OnlineActivations" (
    "Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "ActivationKeyId" UUID NOT NULL REFERENCES "ActivationKeys"("Id") ON DELETE CASCADE,
    "MachineFingerprint" VARCHAR(256) NOT NULL,
    "MachineName" VARCHAR(256),
    "Token" VARCHAR(512) NOT NULL UNIQUE,
    "ActivatedAt" TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "LastHeartbeat" TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "ExpiresAt" TIMESTAMP WITH TIME ZONE,
    "DeactivatedAt" TIMESTAMP WITH TIME ZONE,
    "IsActive" BOOLEAN NOT NULL DEFAULT TRUE
);

CREATE INDEX idx_online_activations_key ON "OnlineActivations" ("ActivationKeyId");
CREATE INDEX idx_online_activations_token ON "OnlineActivations" ("Token");
CREATE INDEX idx_online_activations_fingerprint ON "OnlineActivations" ("MachineFingerprint");
CREATE INDEX idx_online_activations_heartbeat ON "OnlineActivations" ("LastHeartbeat") WHERE "IsActive" = TRUE;
