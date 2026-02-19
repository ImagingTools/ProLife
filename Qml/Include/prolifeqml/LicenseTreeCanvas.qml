import QtQuick 2.15
import imtcontrols 1.0

Item {
	id: root
	
	property var treeData: null
	property string currentLicenseId: ""  // ID of the license being edited
	
	// Trigger repaint when currentLicenseId changes
	onCurrentLicenseIdChanged: {
		canvas.requestPaint();
	}
	
	property int nodeWidth: 180
	property int nodeHeight: 90
	property int horizontalSpacing: 50
	property int verticalSpacing: 60
	
	readonly property int ellipsisWidthMargin: 20
	readonly property int arrowSize: 8
	
	// Modern color scheme
	readonly property color currentNodeColor: "#4A90E2"
	readonly property color arrowColor: "#6C757D"
	readonly property color revokeArrowColor: "#DC3545"  // Red for revoke operations
	readonly property color transferTextColor: "#28A745"
	readonly property color revokeTextColor: "#DC3545"
	
	Canvas {
		id: canvas
		anchors.fill: parent
		
		onPaint: {
			if (!root.treeData) {
				return;
			}
			
			let ctx = getContext("2d");
			ctx.clearRect(0, 0, width, height);
			
			// Calculate tree layout
			let layout = calculateLayout(root.treeData);
			
			// Assign X coordinates to prevent overlapping
			assignXCoordinates(layout, 20);
			
			// Draw connections and arrows first (so they appear behind nodes)
			drawConnections(ctx, layout);
			
			// Draw revoke edges
			drawRevokeEdges(ctx, layout);
			
			// Draw nodes
			drawNodes(ctx, layout);
		}
		
		function calculateLayout(node, level) {
			if (!node) return null;
			
			level = level || 0;
			
			let nodeInfo = {
				node: node,
				level: level,
				x: 0,  // Will be assigned by assignXCoordinates
				y: level * (root.nodeHeight + root.verticalSpacing) + 20,
				children: []
			};
			
			// Calculate children layouts
			if (node.m_children && node.m_children.count > 0) {
				for (let i = 0; i < node.m_children.count; i++) {
					let childItem = node.m_children.get(i).item
					let childLayout = calculateLayout(childItem, level + 1);
					if (childLayout) {
						nodeInfo.children.push(childLayout);
					}
				}
			}
			
			return nodeInfo;
		}
		
		function assignXCoordinates(layout, startX) {
			if (!layout) return startX;
			
			let currentX = startX;
			
			if (layout.children.length === 0) {
				layout.x = currentX;
				return currentX + root.nodeWidth + root.horizontalSpacing;
			}
			
			// Position children first
			for (let i = 0; i < layout.children.length; i++) {
				currentX = assignXCoordinates(layout.children[i], currentX);
			}
			
			// Position this node centered above children
			if (layout.children.length > 0) {
				let leftmost = layout.children[0];
				let rightmost = layout.children[layout.children.length - 1];
				layout.x = (leftmost.x + rightmost.x) / 2;
			}
			
			return currentX;
		}
		
		function calculateOriginalParentCount(layout) {
			let total = layout.node.m_productCount;
			for (let j = 0; j < layout.children.length; j++) {
				total += layout.children[j].node.m_productCount;
			}
			return total;
		}
		
		function drawConnections(ctx, layout) {
			if (!layout || !layout.children || layout.children.length === 0) {
				return;
			}
			
			let parentCenterX = layout.x + root.nodeWidth / 2;
			let parentBottomY = layout.y + root.nodeHeight;
			
			ctx.strokeStyle = root.arrowColor;
			ctx.fillStyle = root.arrowColor;
			ctx.lineWidth = 2;
			
			// Calculate original parent count (before any splits)
			let originalParentCount = calculateOriginalParentCount(layout);
			
			// First pass: Draw all lines and arrows
			for (let i = 0; i < layout.children.length; i++) {
				let child = layout.children[i];
				let childCenterX = child.x + root.nodeWidth / 2;
				let childTopY = child.y;
				
				// Draw line from parent to child
				ctx.strokeStyle = root.arrowColor;
				ctx.fillStyle = root.arrowColor;
				ctx.beginPath();
				ctx.moveTo(parentCenterX, parentBottomY);
				ctx.lineTo(parentCenterX, parentBottomY + root.verticalSpacing / 2);
				ctx.lineTo(childCenterX, parentBottomY + root.verticalSpacing / 2);
				ctx.lineTo(childCenterX, childTopY);
				ctx.stroke();
				
				// Draw arrow at child
				let arrowY = childTopY;
				ctx.beginPath();
				ctx.moveTo(childCenterX, arrowY);
				ctx.lineTo(childCenterX - root.arrowSize, arrowY - root.arrowSize);
				ctx.lineTo(childCenterX + root.arrowSize, arrowY - root.arrowSize);
				ctx.closePath();
				ctx.fill();
			}
			
			// Second pass: Draw all labels on top of lines
			// If there are multiple children, show the total count on the parent's outgoing line
			if (layout.children.length > 1) {
				let midY = parentBottomY + root.verticalSpacing / 4;
				
				ctx.fillStyle = root.transferTextColor;
				ctx.font = "bold 11px " + Style.fontFamily;
				ctx.textAlign = "center";
				ctx.textBaseline = "middle";
				let totalText = originalParentCount.toString();
				
				// Draw background for total text
				let totalTextWidth = ctx.measureText(totalText).width;
				ctx.fillStyle = Style.backgroundColor;
				ctx.fillRect(parentCenterX - totalTextWidth / 2 - 4, midY - 8, totalTextWidth + 8, 16);
				
				ctx.fillStyle = root.transferTextColor;
				ctx.fillText(totalText, parentCenterX, midY);
			}
			
			// Draw individual child transfer labels
			for (let i = 0; i < layout.children.length; i++) {
				let child = layout.children[i];
				let childCenterX = child.x + root.nodeWidth / 2;
				
				// Draw transfer info
				ctx.fillStyle = root.transferTextColor;
				ctx.font = "bold 11px " + Style.fontFamily;
				ctx.textAlign = "center";
				ctx.textBaseline = "middle";
				
				// If parent has only 1 child, show full transfer (originalCount → childCount)
				// If parent has multiple children, show only the individual transfer amount
				let transferText;
				if (layout.children.length === 1) {
					transferText = originalParentCount + " → " + child.node.m_productCount;
				} else {
					transferText = child.node.m_productCount.toString();
				}
				
				let transferY = parentBottomY + root.verticalSpacing / 2;
				
				// Draw background for transfer text
				let textWidth = ctx.measureText(transferText).width;
				ctx.fillStyle = Style.backgroundColor;
				ctx.fillRect(childCenterX - textWidth / 2 - 4, transferY - 8, textWidth + 8, 16);
				
				ctx.fillStyle = root.transferTextColor;
				ctx.fillText(transferText, childCenterX, transferY);
			}
			
			// Third pass: Recursively draw child connections
			for (let i = 0; i < layout.children.length; i++) {
				drawConnections(ctx, layout.children[i]);
			}
		}
		
		function drawRevokeEdges(ctx, layout) {
			if (!layout) return;
			
			// Create a map of node IDs to their layout info for quick lookup
			let nodeMap = {};
			
			let collectNodes = function(nodeLayout) {
				if (!nodeLayout) return;
				nodeMap[nodeLayout.node.m_id] = nodeLayout;
				if (nodeLayout.children) {
					for (let i = 0; i < nodeLayout.children.length; i++) {
						collectNodes(nodeLayout.children[i]);
					}
				}
			}
			
			collectNodes(layout);
			
			// Draw revoke edges for all nodes
			let  drawNodeRevokeEdges = function(nodeLayout) {
				if (!nodeLayout) return;
				
				// Check if this node has revoke edges
				if (nodeLayout.node.m_revokeEdges && nodeLayout.node.m_revokeEdges.count > 0) {
					for (let i = 0; i < nodeLayout.node.m_revokeEdges.count; i++) {
						let revokeEdge = nodeLayout.node.m_revokeEdges.get(i).item;
						let fromLayout = nodeMap[revokeEdge.m_fromNodeId];
						let toLayout = nodeMap[revokeEdge.m_toNodeId];
						
						if (fromLayout && toLayout) {
							// Calculate positions
							let fromX = fromLayout.x + root.nodeWidth / 2;
							let fromY = fromLayout.y + root.nodeHeight / 2;
							let toX = toLayout.x + root.nodeWidth / 2;
							let toY = toLayout.y + root.nodeHeight / 2;
							
							// Draw curved red line
							ctx.strokeStyle = root.revokeArrowColor;
							ctx.fillStyle = root.revokeArrowColor;
							ctx.lineWidth = 3;
							ctx.setLineDash([5, 5]);  // Dashed line
							
							// Draw bezier curve
							ctx.beginPath();
							ctx.moveTo(fromX, fromY);
							let controlX = (fromX + toX) / 2 + root.horizontalSpacing;
							let controlY = (fromY + toY) / 2;
							ctx.quadraticCurveTo(controlX, controlY, toX, toY);
							ctx.stroke();
							ctx.setLineDash([]);  // Reset line dash
							
							// Draw arrow at destination
							let angle = Math.atan2(toY - controlY, toX - controlX);
							ctx.beginPath();
							ctx.moveTo(toX, toY);
							ctx.lineTo(toX - root.arrowSize * Math.cos(angle - Math.PI / 6), 
							          toY - root.arrowSize * Math.sin(angle - Math.PI / 6));
							ctx.lineTo(toX - root.arrowSize * Math.cos(angle + Math.PI / 6), 
							          toY - root.arrowSize * Math.sin(angle + Math.PI / 6));
							ctx.closePath();
							ctx.fill();
							
							// Draw revoke count label (red number only, no "REVOKED:" text)
							// Position offset from curve to avoid overlapping with split labels
							let labelX = controlX + 20;
							let labelY = controlY - 15;
							ctx.fillStyle = root.revokeTextColor;
							ctx.font = "bold 12px " + Style.fontFamily;
							ctx.textAlign = "center";
							ctx.textBaseline = "middle";
							let revokeText = revokeEdge.m_revokedCount.toString();
							
							// Draw background
							let textWidth = ctx.measureText(revokeText).width;
							ctx.fillStyle = Style.backgroundColor;
							ctx.fillRect(labelX - textWidth / 2 - 4, labelY - 8, textWidth + 8, 16);
							
							ctx.fillStyle = root.revokeTextColor;
							ctx.fillText(revokeText, labelX, labelY);
						}
					}
				}
				
				// Recursively process children
				if (nodeLayout.children) {
					for (let i = 0; i < nodeLayout.children.length; i++) {
						drawNodeRevokeEdges(nodeLayout.children[i]);
					}
				}
			}
			
			drawNodeRevokeEdges(layout);
		}
		
		function drawNodes(ctx, layout) {
			if (!layout) return;
			
			let node = layout.node;
			let x = layout.x;
			let y = layout.y;
			
			// Determine if this is the current license
			let isCurrent = (node.m_id === root.currentLicenseId);
			
			// Modern styling with rounded corners (simulated with shadow)
			// Draw shadow first for current node
			if (isCurrent) {
				ctx.fillStyle = "rgba(74, 144, 226, 0.2)";
				drawRoundedRect(ctx, x + 4, y + 4, root.nodeWidth, root.nodeHeight, Style.radiusM);
				ctx.fill();
			}
			
			// Draw node rectangle with rounded corners using Style properties
			let bgColor = isCurrent ? root.currentNodeColor : Style.baseColor;
			let borderColor = Style.borderColor;
			
			ctx.fillStyle = bgColor;
			ctx.strokeStyle = borderColor;
			ctx.lineWidth = 1;
			
			drawRoundedRect(ctx, x, y, root.nodeWidth, root.nodeHeight, Style.radiusM);
			ctx.fill();
			ctx.stroke();
			
			// Draw "CURRENT" badge if this is the current license
			if (isCurrent) {
				ctx.fillStyle = "#FFC107";
				ctx.fillRect(x + root.nodeWidth - 70, y, 70, 24);
				ctx.fillStyle = "#000";
				ctx.font = "bold 10px " + Style.fontFamily;
				ctx.textAlign = "center";
				ctx.textBaseline = "middle";
				ctx.fillText("CURRENT", x + root.nodeWidth - 35, y + 12);
			}
			
			// Draw "REVOKED" indicator if licenses were revoked (transferredCount > productCount)
			let hasRevoke = (node.m_transferredCount || 0) > (node.m_productCount || 0);
			if (hasRevoke && !isCurrent) {
				ctx.fillStyle = root.revokeArrowColor;
				let badgeX = x + root.nodeWidth - 75;
				ctx.fillRect(badgeX, y, 75, 24);
				ctx.fillStyle = "#FFF";
				ctx.font = "bold 10px " + Style.fontFamily;
				ctx.textAlign = "center";
				ctx.textBaseline = "middle";
				let revokedAmount = node.m_transferredCount - node.m_productCount;
				ctx.fillText("- " + revokedAmount, badgeX + 37, y + 12);
			}
			
			// Draw text
			ctx.fillStyle = isCurrent ? "#FFFFFF" : Style.textColor;
			ctx.textAlign = "left";
			ctx.textBaseline = "top";
			
			let textX = x + 10;
			let textY = y + 15;
			let lineHeight = 20;
			
			// Serial number (used as name)
			if (node.m_serialNumber) {
				ctx.font = "bold 14px " + Style.fontFamily;
				ctx.fillText(truncateText(ctx, node.m_serialNumber, root.nodeWidth - 20), textX, textY);
				textY += lineHeight + 5;
			}
			
			// Account info
			if (node.m_accountName) {
				ctx.font = "11px " + Style.fontFamily;
				ctx.fillStyle = isCurrent ? "rgba(255, 255, 255, 0.8)" : "#6C757D";
				ctx.fillText(truncateText(ctx, "Account: " + node.m_accountName, root.nodeWidth - 20), textX, textY);
				textY += lineHeight;
			}
			
			// Count info
			ctx.font = "bold 12px " + Style.fontFamily;
			ctx.fillStyle = isCurrent ? "#FFFFFF" : Style.textColor;
			let countText = "Licenses: " + (node.m_productCount || 0);
			
			// If licenses were revoked, show original count too
			if (hasRevoke) {
				countText += " (was " + node.m_transferredCount + ")";
			}
			
			ctx.fillText(truncateText(ctx, countText, root.nodeWidth - 20), textX, textY);
			
			// Draw children
			if (layout.children) {
				for (let i = 0; i < layout.children.length; i++) {
					drawNodes(ctx, layout.children[i]);
				}
			}
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
		
		function truncateText(ctx, text, maxWidth) {
			if (!text) return "";
			let width = ctx.measureText(text).width;
			if (width <= maxWidth) {
				return text;
			}
			
			while (width > maxWidth - root.ellipsisWidthMargin && text.length > 0) {
				text = text.substring(0, text.length - 1);
				width = ctx.measureText(text + "...").width;
			}
			return text + "...";
		}
	}
	
	onTreeDataChanged: {
		if (treeData) {
			// Recalculate layout with proper X coordinates
			let layout = canvas.calculateLayout(treeData);
			if (layout) {
				canvas.assignXCoordinates(layout, 20);
				
				// Calculate required canvas size
				let maxX = calculateMaxX(layout);
				let maxY = calculateMaxY(layout);
				
				root.width = maxX + root.nodeWidth + 40;
				root.height = maxY + root.nodeHeight + 40;
			}
			
			canvas.requestPaint();
		}
	}
	
	function calculateMaxX(layout) {
		if (!layout) return 0;
		
		let maxX = layout.x;
		
		if (layout.children) {
			for (let i = 0; i < layout.children.length; i++) {
				maxX = Math.max(maxX, calculateMaxX(layout.children[i]));
			}
		}
		
		return maxX;
	}
	
	function calculateMaxY(layout) {
		if (!layout) return 0;
		
		let maxY = layout.y;
		
		if (layout.children) {
			for (let i = 0; i < layout.children.length; i++) {
				maxY = Math.max(maxY, calculateMaxY(layout.children[i]));
			}
		}
		
		return maxY;
	}
}
