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
	property int horizontalSpacing: 30  // Reduced from 50 to bring nodes closer
	property int verticalSpacing: 60
	
	readonly property int ellipsisWidthMargin: 20
	readonly property int arrowSize: 8

	// Actual tree dimensions (not capped)
	property int treeWidth: 0
	property int treeHeight: 0
	property int contentOffsetX: 0  // Horizontal offset for centering
	
	readonly property int legendHeight: 50  // Height of legend + margin

	// Modern color scheme
	readonly property color currentNodeColor: "#4A90E2"
	readonly property color arrowColor: "#6C757D"
	readonly property color revokeArrowColor: "#DC3545"  // Red for revoke operations
	readonly property color transferTextColor: "#28A745"
	readonly property color revokeTextColor: "#DC3545"
	
	// License count colors for (A/B/C) format
	readonly property color availableCountColor: "#28A745"  // Green for available
	readonly property color boundCountColor: "#FFC107"      // Amber for bound
	readonly property color totalCountColor: "#6C757D"      // Gray for total

	onTreeDataChanged: {
		updateContentDimensions();
	}

	// Recalculate centering when width changes
	onWidthChanged: {
		if (treeData) {
			updateContentDimensions();
		}
	}

	function updateContentDimensions() {
		if (!treeData) {
			treeWidth = 0;
			treeHeight = 0;
			return;
		}
		
		// Calculate tree layout to get dimensions
		let layout = canvas.calculateLayout(treeData);
		canvas.assignXCoordinates(layout, 20);
		
		// Get tree bounds (actual tree size, not capped)
		let bounds = canvas.getTreeBounds(layout);
		treeWidth = bounds.maxX + 20;
		treeHeight = bounds.maxY + 20;
		
		// Calculate horizontal offset to center canvas within assigned width
		contentOffsetX = Math.max(0, (root.width - treeWidth) / 2);
		
		canvas.requestPaint();
	}
	
	// Legend at the top
	Rectangle {
		id: legend
		width: parent.width
		height: 40
		color: Style.baseColor
		border.color: Style.borderColor
		border.width: 1
		z: 10
		
		Row {
			anchors.centerIn: parent
			spacing: 30
			
			Row {
				spacing: 8
				Text {
					text: "●"
					color: root.availableCountColor
					font.pixelSize: 16
					anchors.verticalCenter: parent.verticalCenter
				}
				Text {
					text: "Available"
					color: Style.textColor
					font.family: Style.fontFamily
					font.pixelSize: 12
					anchors.verticalCenter: parent.verticalCenter
				}
			}
			
			Row {
				spacing: 8
				Text {
					text: "●"
					color: root.boundCountColor
					font.pixelSize: 16
					anchors.verticalCenter: parent.verticalCenter
				}
				Text {
					text: "Bound"
					color: Style.textColor
					font.family: Style.fontFamily
					font.pixelSize: 12
					anchors.verticalCenter: parent.verticalCenter
				}
			}
			
			Row {
				spacing: 8
				Text {
					text: "●"
					color: root.totalCountColor
					font.pixelSize: 16
					anchors.verticalCenter: parent.verticalCenter
				}
				Text {
					text: "Total"
					color: Style.textColor
					font.family: Style.fontFamily
					font.pixelSize: 12
					anchors.verticalCenter: parent.verticalCenter
				}
			}
		}
	}
	
	Canvas {
		id: canvas
		width: root.treeWidth
		height: root.treeHeight
		x: root.contentOffsetX
		y: legend.height + 10  // Position below legend with margin

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
			
			// Draw nodes
			drawNodes(ctx, layout);
			
			// Draw revoke edges after nodes so arrows are visible on top
			// drawRevokeEdges(ctx, layout);
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
		
		function getTreeBounds(layout) {
			if (!layout) {
				return { minX: 0, minY: 0, maxX: 0, maxY: 0 };
			}
			
			let bounds = {
				minX: layout.x,
				minY: layout.y,
				maxX: layout.x + root.nodeWidth,
				maxY: layout.y + root.nodeHeight
			};
			
			// Check all children recursively
			for (let i = 0; i < layout.children.length; i++) {
				let childBounds = getTreeBounds(layout.children[i]);
				bounds.minX = Math.min(bounds.minX, childBounds.minX);
				bounds.minY = Math.min(bounds.minY, childBounds.minY);
				bounds.maxX = Math.max(bounds.maxX, childBounds.maxX);
				bounds.maxY = Math.max(bounds.maxY, childBounds.maxY);
			}
			
			return bounds;
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
			
			// Second pass: Draw individual transfer labels on top of lines
			for (let i = 0; i < layout.children.length; i++) {
				let child = layout.children[i];
				let childCenterX = child.x + root.nodeWidth / 2;
				
				// Draw transfer info using pre-calculated values from server
				ctx.fillStyle = root.transferTextColor;
				ctx.font = "bold 11px " + Style.fontFamily;
				ctx.textAlign = "center";
				ctx.textBaseline = "middle";
				
				// Show only the transferred amount (how many licenses were transferred to this child)
				let transferText;
				if (child.node.m_transferredCount !== undefined) {
					transferText = child.node.m_transferredCount.toString();
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
							// Calculate edge positions based on relative positions
							// Connect from center of child's side to center of parent's side
							// to avoid overlapping with split arrows in the center
							
							var fromX;
							var toX;
							var fromY;
							var toY;
							
							// Determine which side to use based on relative horizontal positions
							if (fromLayout.x < toLayout.x) {
								// Child is to the left of parent
								// Connect from center of child's LEFT side to center of parent's LEFT side
								fromX = fromLayout.x;  // Left edge of child
								fromY = fromLayout.y + root.nodeHeight / 2;  // Vertical center of child
								toX = toLayout.x;  // Left edge of parent
								toY = toLayout.y + root.nodeHeight / 2;  // Vertical center of parent
							} else if (fromLayout.x > toLayout.x) {
								// Child is to the right of parent
								// Connect from center of child's RIGHT side to center of parent's RIGHT side
								fromX = fromLayout.x + root.nodeWidth;  // Right edge of child
								fromY = fromLayout.y + root.nodeHeight / 2;  // Vertical center of child
								toX = toLayout.x + root.nodeWidth;  // Right edge of parent
								toY = toLayout.y + root.nodeHeight / 2;  // Vertical center of parent
							} else {
								// Child is directly below parent (same X position)
								// Connect from center of child's top to center of parent's bottom
								fromX = fromLayout.x + root.nodeWidth / 2;  // Horizontal center of child
								fromY = fromLayout.y;  // Top edge of child
								toX = toLayout.x + root.nodeWidth / 2;  // Horizontal center of parent
								toY = toLayout.y + root.nodeHeight;  // Bottom edge of parent
							}
							
							// Draw straight red dashed line
							ctx.strokeStyle = root.revokeArrowColor;
							ctx.fillStyle = root.revokeArrowColor;
							ctx.lineWidth = 3;
							ctx.setLineDash([5, 5]);  // Dashed line
							
							// Draw line (horizontal for side connections, with corner for top-bottom)
							ctx.beginPath();
							ctx.moveTo(fromX, fromY);
							
							if (fromLayout.x !== toLayout.x) {
								// Horizontal connection (side to side)
								var midX = (fromX + toX) / 2;
								ctx.lineTo(midX, fromY);
								ctx.lineTo(midX, toY);
								ctx.lineTo(toX, toY);
							} else {
								// Vertical connection (top to bottom)
								ctx.lineTo(toX, toY);
							}
							ctx.stroke();
							
							// Draw filled arrow at parent pointing to it
							ctx.setLineDash([]);  // Reset dash for solid arrow
							ctx.beginPath();
							
							if (fromLayout.x < toLayout.x) {
								// Child left of parent - arrow pointing up on left side
								ctx.moveTo(toX, toY);
								ctx.lineTo(toX - root.arrowSize, toY - root.arrowSize);
								ctx.lineTo(toX + root.arrowSize, toY - root.arrowSize);
							} else if (fromLayout.x > toLayout.x) {
								// Child right of parent - arrow pointing up on right side
								ctx.moveTo(toX, toY);
								ctx.lineTo(toX - root.arrowSize, toY - root.arrowSize);
								ctx.lineTo(toX + root.arrowSize, toY - root.arrowSize);
							} else {
								// Arrow pointing up (into parent's bottom)
								ctx.moveTo(toX, toY);
								ctx.lineTo(toX - root.arrowSize, toY - root.arrowSize);
								ctx.lineTo(toX + root.arrowSize, toY - root.arrowSize);
							}
							ctx.closePath();
							ctx.fill();
							
							ctx.setLineDash([]);  // Reset line dash
							
							// Draw revoke count label (red number only)
							// Position on the line to avoid overlapping
							var labelX;
							var labelY;
							
							if (fromLayout.x !== toLayout.x) {
								// For horizontal connections, put label on horizontal segment
								labelX = (fromX + toX) / 2;
								labelY = (fromY + toY) / 2 - 10;
							} else {
								// For vertical connections, put label next to line
								labelX = toX + 15;
								labelY = (fromY + toY) / 2;
							}
							
							ctx.fillStyle = root.revokeTextColor;
							ctx.font = "bold 12px " + Style.fontFamily;
							ctx.textAlign = "center";
							ctx.textBaseline = "middle";
							var revokeText = revokeEdge.m_revokedCount.toString();
							
							// Draw background
							var textWidth = ctx.measureText(revokeText).width;
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
			
			// Draw text
			ctx.fillStyle = isCurrent ? "#FFFFFF" : Style.textColor;
			ctx.textAlign = "left";
			ctx.textBaseline = "top";
			
			let textX = x + 10;
			let textY = y + 15;
			let lineHeight = 20;
			
			// Serial number (used as name) - wrap if too long
			if (node.m_serialNumber) {
				ctx.font = "bold 14px " + Style.fontFamily;
				let serialText = node.m_serialNumber;
				let maxWidth = root.nodeWidth - 20;
				
				// If serial number is too long, try to wrap it intelligently
				if (ctx.measureText(serialText).width > maxWidth) {
					// Try to split on dash or hyphen for hierarchical serial numbers
					let parts = serialText.split(/[-_]/);
					if (parts.length > 1 && parts[0].length > 0) {
						// Draw first part on first line
						ctx.fillText(truncateText(ctx, parts[0] + "-", maxWidth), textX, textY);
						textY += lineHeight;
						// Draw remaining parts on second line
						let remaining = parts.slice(1).join("-");
						ctx.fillText(truncateText(ctx, remaining, maxWidth), textX, textY);
						textY += lineHeight;
					} else {
						// No good split point, just truncate
						ctx.fillText(truncateText(ctx, serialText, maxWidth), textX, textY);
						textY += lineHeight + 5;
					}
				} else {
					// Fits on one line
					ctx.fillText(serialText, textX, textY);
					textY += lineHeight + 5;
				}
			}
			
			// Account info
			if (node.m_accountName) {
				ctx.font = "11px " + Style.fontFamily;
				ctx.fillStyle = isCurrent ? "rgba(255, 255, 255, 0.8)" : "#6C757D";
				ctx.fillText(truncateText(ctx, "Account: " + node.m_accountName, root.nodeWidth - 20), textX, textY);
				textY += lineHeight;
			}
			
			// Count info - display as (A/B/C) format with colors
			// A = available (free), B = bound, C = total
			let totalCount = node.m_transferredCount || node.m_initialCount || node.m_productCount || 0;
			let boundCount = node.m_boundCount || 0;
			let remainingCount = node.m_productCount || 0;
			let availableCount = remainingCount - boundCount;
			
			// Draw label
			ctx.font = "11px " + Style.fontFamily;
			ctx.fillStyle = isCurrent ? "rgba(255, 255, 255, 0.8)" : "#6C757D";
			ctx.fillText("Licenses: (", textX, textY);
			
			// Calculate positions for colored numbers
			let labelWidth = ctx.measureText("Licenses: (").width;
			let currentX = textX + labelWidth;
			
			// Draw available count in green
			ctx.font = "bold 12px " + Style.fontFamily;
			ctx.fillStyle = isCurrent ? "#FFFFFF" : root.availableCountColor;
			let availableText = availableCount.toString();
			ctx.fillText(availableText, currentX, textY);
			currentX += ctx.measureText(availableText).width;
			
			// Draw separator
			ctx.font = "11px " + Style.fontFamily;
			ctx.fillStyle = isCurrent ? "rgba(255, 255, 255, 0.8)" : "#6C757D";
			ctx.fillText("/", currentX, textY);
			currentX += ctx.measureText("/").width;
			
			// Draw bound count in amber
			ctx.font = "bold 12px " + Style.fontFamily;
			ctx.fillStyle = isCurrent ? "#FFFFFF" : root.boundCountColor;
			let boundText = boundCount.toString();
			ctx.fillText(boundText, currentX, textY);
			currentX += ctx.measureText(boundText).width;
			
			// Draw separator
			ctx.font = "11px " + Style.fontFamily;
			ctx.fillStyle = isCurrent ? "rgba(255, 255, 255, 0.8)" : "#6C757D";
			ctx.fillText("/", currentX, textY);
			currentX += ctx.measureText("/").width;
			
			// Draw total count in gray
			ctx.font = "bold 12px " + Style.fontFamily;
			ctx.fillStyle = isCurrent ? "#FFFFFF" : root.totalCountColor;
			let totalText = totalCount.toString();
			ctx.fillText(totalText, currentX, textY);
			currentX += ctx.measureText(totalText).width;
			
			// Draw closing parenthesis
			ctx.font = "11px " + Style.fontFamily;
			ctx.fillStyle = isCurrent ? "rgba(255, 255, 255, 0.8)" : "#6C757D";
			ctx.fillText(")", currentX, textY);
			
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
	
	// onTreeDataChanged: {
	// 	if (treeData) {
	// 		// Recalculate layout with proper X coordinates
	// 		let layout = canvas.calculateLayout(treeData);
	// 		if (layout) {
	// 			canvas.assignXCoordinates(layout, 20);
				
	// 			// Calculate required canvas size
	// 			let maxX = calculateMaxX(layout);
	// 			let maxY = calculateMaxY(layout);
				
	// 			root.width = maxX + root.nodeWidth + 40;
	// 			root.height = maxY + root.nodeHeight + 40;
	// 		}
			
	// 		canvas.requestPaint();
	// 	}
	// }
	
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
