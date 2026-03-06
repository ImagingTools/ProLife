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
	property int layoutStartY: 20
	property bool singleColumnTree: false
	
	// Modern color scheme
	readonly property color currentNodeColor: "#4A90E2"
	readonly property color arrowColor: "#6C757D"
	readonly property color revokeArrowColor: "#DC3545"  // Red for revoke operations
	readonly property color transferTextColor: "#6C757D"  // Gray, same as total count
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

	function isSingleColumnTree(node) {
		if (!node) {
			return true;
		}

		if (node.m_children && node.m_children.count > 1) {
			return false;
		}

		if (node.m_children && node.m_children.count === 1) {
			return isSingleColumnTree(node.m_children.get(0).item);
		}

		return true;
	}

	function updateContentDimensions() {
		if (!treeData) {
			treeWidth = 0;
			treeHeight = 0;
			return;
		}

		singleColumnTree = isSingleColumnTree(treeData);
		layoutStartY = singleColumnTree ? (legend.height + legend.anchors.topMargin + 10) : 20;

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
	
	// Legend in top-left corner (vertical layout, no border)
	Item {
		id: legend
		width: 80  // Width for vertical layout
		height: 70  // Height for 3 items vertically
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 10
		anchors.leftMargin: 10
		z: 10
		
		Column {
			spacing: 3
			
			Row {
				spacing: 5
				Text {
					text: "●"
					color: root.availableCountColor
					font.pixelSize: 12
					anchors.verticalCenter: parent.verticalCenter
				}
				Text {
					text: "Available"
					color: Style.textColor
					font.family: Style.fontFamily
					font.pixelSize: 10
					anchors.verticalCenter: parent.verticalCenter
				}
			}
			
			Row {
				spacing: 5
				Text {
					text: "●"
					color: root.boundCountColor
					font.pixelSize: 12
					anchors.verticalCenter: parent.verticalCenter
				}
				Text {
					text: "Bound"
					color: Style.textColor
					font.family: Style.fontFamily
					font.pixelSize: 10
					anchors.verticalCenter: parent.verticalCenter
				}
			}
			
			Row {
				spacing: 5
				Text {
					text: "●"
					color: root.totalCountColor
					font.pixelSize: 12
					anchors.verticalCenter: parent.verticalCenter
				}
				Text {
					text: "Total"
					color: Style.textColor
					font.family: Style.fontFamily
					font.pixelSize: 10
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
		y: 0  // Legend is floating, doesn't affect canvas position

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
				y: level * (root.nodeHeight + root.verticalSpacing) + root.layoutStartY,
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
			// A = available (not bound, not split out), B = bound, C = total allocated
			let totalCount = node.m_transferredCount || node.m_initialCount || node.m_productCount || 0;
			let boundCount = node.m_boundCount || 0;
			let productCount = node.m_productCount || 0;  // Remaining after splits/revokes
			let availableCount = productCount - boundCount;  // Free licenses at this node
			
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
