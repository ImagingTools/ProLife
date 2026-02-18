import QtQuick 2.15
import imtcontrols 1.0

Item {
	id: root

	property var treeData: null  // Array of LicenseTreeNode union types
	property string currentLicenseId: ""  // ID of the license being edited
	
	// Trigger layout recalculation when data changes
	onTreeDataChanged: {
		updateTimeline();
	}
	
	onCurrentLicenseIdChanged: {
		updateTimeline();
	}
	
	// Timeline properties
	property int nodeWidth: 300
	property int nodeHeight: 100
	property int nodeSpacing: 40
	property int leftMargin: 50
	property int topMargin: 30
	
	// Modern color scheme
	readonly property color splitColor: "#4A90E2"  // Blue for split
	readonly property color revokeColor: "#DC3545"  // Red for revoke
	readonly property color outgoingColor: "#FFA500"  // Orange for outgoing
	readonly property color incomingColor: "#28A745"  // Green for incoming
	readonly property color currentNodeColor: "#FFD700"  // Gold highlight
	
	function updateTimeline() {
		canvas.requestPaint()
	}
	
	Canvas {
		id: canvas
		anchors.fill: parent
		
		onPaint: {
			if (!root.treeData || root.treeData.count === 0) {
				return;
			}

			let ctx = getContext("2d");
			ctx.clearRect(0, 0, width, height);
			
			// Draw timeline from top to bottom
			let yPos = root.topMargin;
			
			for (let i = 0; i < root.treeData.count; i++) {
				let node = root.treeData.get(i).item;
				let xPos = root.leftMargin;
				
				// Determine node type and draw accordingly
				if (node.m_nodeType === "SplitOut"){
					drawSplitOutNode(ctx, node, xPos, yPos);
				} else if (node.m_nodeType === "SplitIn") {
					drawSplitInNode(ctx, node, xPos, yPos);
				} else if (node.m_nodeType === "RevokeOut") {
					drawRevokeOutNode(ctx, node, xPos, yPos);
				} else if (node.m_nodeType === "RevokeIn") {
					drawRevokeInNode(ctx, node, xPos, yPos);
				}
				
				yPos += root.nodeHeight + root.nodeSpacing;
			}
		}
		
		function drawSplitOutNode(ctx, node, x, y) {
			// Draw node box
			ctx.fillStyle = root.splitColor;
			ctx.strokeStyle = "#2A5A8A";
			ctx.lineWidth = 2;
			
			drawRoundedRect(ctx, x, y, root.nodeWidth, root.nodeHeight, 8);
			ctx.fill();
			ctx.stroke();
			
			// Draw text
			ctx.fillStyle = "#FFFFFF";
			ctx.font = "bold 14px Arial";
			ctx.fillText("SPLIT OUT", x + 10, y + 25);
			
			ctx.font = "12px Arial";
			ctx.fillText("→ To License: " + (node.m_newLicenseId || "N/A"), x + 10, y + 50);
			ctx.fillText("Initial: " + (node.m_initialCount || 0) + " → Moved: " + (node.m_movedCount || 0), x + 10, y + 70);
			
			// Indicator icon
			drawArrowIcon(ctx, x + root.nodeWidth - 35, y + 10, root.outgoingColor);
		}
		
		function drawSplitInNode(ctx, node, x, y) {
			// Draw node box
			ctx.fillStyle = root.incomingColor;
			ctx.strokeStyle = "#1A6A2A";
			ctx.lineWidth = 2;
			
			drawRoundedRect(ctx, x, y, root.nodeWidth, root.nodeHeight, 8);
			ctx.fill();
			ctx.stroke();
			
			// Draw text
			ctx.fillStyle = "#FFFFFF";
			ctx.font = "bold 14px Arial";
			ctx.fillText("SPLIT IN", x + 10, y + 25);
			
			ctx.font = "12px Arial";
			ctx.fillText("← From License: " + (node.m_sourceLicenseId || "N/A"), x + 10, y + 50);
			ctx.fillText("Received: " + (node.m_receivedCount || 0), x + 10, y + 70);
			
			// Indicator icon
			drawArrowIcon(ctx, x + root.nodeWidth - 35, y + 10, root.incomingColor);
		}
		
		function drawRevokeOutNode(ctx, node, x, y) {
			// Draw node box
			ctx.fillStyle = root.revokeColor;
			ctx.strokeStyle = "#8A2A2A";
			ctx.lineWidth = 2;
			
			drawRoundedRect(ctx, x, y, root.nodeWidth, root.nodeHeight, 8);
			ctx.fill();
			ctx.stroke();
			
			// Draw text
			ctx.fillStyle = "#FFFFFF";
			ctx.font = "bold 14px Arial";
			ctx.fillText("REVOKE OUT", x + 10, y + 25);
			
			ctx.font = "12px Arial";
			ctx.fillText("↩ To Parent: " + (node.m_parentLicenseId || "N/A"), x + 10, y + 50);
			ctx.fillText("Had: " + (node.m_initialCount || 0) + " → Revoked: " + (node.m_revokedCount || 0), x + 10, y + 70);
			
			// Indicator icon
			drawRevokeIcon(ctx, x + root.nodeWidth - 35, y + 10);
		}
		
		function drawRevokeInNode(ctx, node, x, y) {
			// Draw node box
			ctx.fillStyle = root.incomingColor;
			ctx.strokeStyle = "#1A6A2A";
			ctx.lineWidth = 2;
			
			drawRoundedRect(ctx, x, y, root.nodeWidth, root.nodeHeight, 8);
			ctx.fill();
			ctx.stroke();
			
			// Draw text
			ctx.fillStyle = "#FFFFFF";
			ctx.font = "bold 14px Arial";
			ctx.fillText("REVOKE IN", x + 10, y + 25);
			
			ctx.font = "12px Arial";
			ctx.fillText("↩ From Child: " + (node.m_childId || "N/A"), x + 10, y + 50);
			ctx.fillText("Now Have: " + (node.m_remainingCount || 0), x + 10, y + 70);
			
			// Indicator icon
			drawArrowIcon(ctx, x + root.nodeWidth - 35, y + 10, root.incomingColor);
		}
		
		function drawRoundedRect(ctx, x, y, width, height, radius) {
			ctx.beginPath();
			ctx.moveTo(x + radius, y);
			ctx.lineTo(x + width - radius, y);
			ctx.arcTo(x + width, y, x + width, y + radius, radius);
			ctx.lineTo(x + width, y + height - radius);
			ctx.arcTo(x + width, y + height, x + width - radius, y + height, radius);
			ctx.lineTo(x + radius, y + height);
			ctx.arcTo(x, y + height, x, y + height - radius, radius);
			ctx.lineTo(x, y + radius);
			ctx.arcTo(x, y, x + radius, y, radius);
			ctx.closePath();
		}
		
		function drawArrowIcon(ctx, x, y, color) {
			ctx.fillStyle = color;
			ctx.beginPath();
			ctx.moveTo(x, y + 10);
			ctx.lineTo(x + 20, y + 10);
			ctx.lineTo(x + 15, y + 5);
			ctx.moveTo(x + 20, y + 10);
			ctx.lineTo(x + 15, y + 15);
			ctx.stroke();
		}
		
		function drawRevokeIcon(ctx, x, y) {
			ctx.strokeStyle = "#FFFFFF";
			ctx.lineWidth = 2;
			ctx.beginPath();
			ctx.arc(x + 10, y + 10, 8, 0, 2 * Math.PI);
			ctx.stroke();
			ctx.beginPath();
			ctx.moveTo(x + 7, y + 7);
			ctx.lineTo(x + 13, y + 13);
			ctx.moveTo(x + 13, y + 7);
			ctx.lineTo(x + 7, y + 13);
			ctx.stroke();
		}
	}
}
