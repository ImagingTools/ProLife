import QtQuick 2.15
import imtcontrols 1.0

Item {
	id: root
	
	property var treeData: null
	property int nodeWidth: 200
	property int nodeHeight: 80
	property int horizontalSpacing: 40
	property int verticalSpacing: 60
	
	readonly property int ellipsisWidthMargin: 20
	
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
			
			// Draw connections first (so they appear behind nodes)
			ctx.strokeStyle = Style.borderColor;
			ctx.lineWidth = 2;
			drawConnections(ctx, layout);
			
			// Draw nodes
			drawNodes(ctx, layout);
		}
		
		function calculateLayout(node, level, index, siblingCount) {
			if (!node) return null;
			
			level = level || 0;
			index = index || 0;
			siblingCount = siblingCount || 1;
			
			let nodeInfo = {
				node: node,
				level: level,
				x: 0,
				y: level * (root.nodeHeight + root.verticalSpacing) + 20,
				children: []
			};
			
			// Calculate children layouts
			if (node.children && node.children.length > 0) {
				for (let i = 0; i < node.children.length; i++) {
					let childLayout = calculateLayout(node.children[i], level + 1, i, node.children.length);
					if (childLayout) {
						nodeInfo.children.push(childLayout);
					}
				}
				
				// Position node centered above its children
				if (nodeInfo.children.length > 0) {
					let leftmost = nodeInfo.children[0];
					let rightmost = nodeInfo.children[nodeInfo.children.length - 1];
					nodeInfo.x = (getNodeX(leftmost) + getNodeX(rightmost)) / 2;
				}
			} else {
				// Leaf node - position based on index
				nodeInfo.x = index * (root.nodeWidth + root.horizontalSpacing) + 20;
			}
			
			return nodeInfo;
		}
		
		function getNodeX(nodeInfo) {
			if (!nodeInfo) return 0;
			return nodeInfo.x;
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
		
		function drawConnections(ctx, layout) {
			if (!layout || !layout.children || layout.children.length === 0) {
				return;
			}
			
			let parentCenterX = layout.x + root.nodeWidth / 2;
			let parentBottomY = layout.y + root.nodeHeight;
			
			for (let i = 0; i < layout.children.length; i++) {
				let child = layout.children[i];
				let childCenterX = child.x + root.nodeWidth / 2;
				let childTopY = child.y;
				
				// Draw line from parent to child
				ctx.beginPath();
				ctx.moveTo(parentCenterX, parentBottomY);
				ctx.lineTo(parentCenterX, parentBottomY + root.verticalSpacing / 2);
				ctx.lineTo(childCenterX, parentBottomY + root.verticalSpacing / 2);
				ctx.lineTo(childCenterX, childTopY);
				ctx.stroke();
				
				// Recursively draw child connections
				drawConnections(ctx, child);
			}
		}
		
		function drawNodes(ctx, layout) {
			if (!layout) return;
			
			let node = layout.node;
			let x = layout.x;
			let y = layout.y;
			
			// Draw node rectangle
			ctx.fillStyle = Style.backgroundColor;
			ctx.strokeStyle = Style.borderColor;
			ctx.lineWidth = 2;
			ctx.fillRect(x, y, root.nodeWidth, root.nodeHeight);
			ctx.strokeRect(x, y, root.nodeWidth, root.nodeHeight);
			
			// Draw text
			ctx.fillStyle = Style.textColor;
			ctx.font = "12px " + Style.fontFamily;
			ctx.textAlign = "center";
			ctx.textBaseline = "top";
			
			let textX = x + root.nodeWidth / 2;
			let textY = y + 5;
			let lineHeight = 16;
			
			// Product name
			if (node.productName) {
				ctx.fillText(truncateText(ctx, node.productName, root.nodeWidth - 10), textX, textY);
				textY += lineHeight;
			}
			
			// Serial number
			if (node.serialNumber) {
				ctx.fillText(truncateText(ctx, node.serialNumber, root.nodeWidth - 10), textX, textY);
				textY += lineHeight;
			}
			
			// Project
			if (node.project) {
				ctx.font = "10px " + Style.fontFamily;
				ctx.fillText(truncateText(ctx, "(" + node.project + ")", root.nodeWidth - 10), textX, textY);
				textY += lineHeight;
			}
			
			// Count info
			ctx.font = "10px " + Style.fontFamily;
			let countText = "Total: " + (node.productCount || 0);
			if (node.availableCount !== undefined) {
				countText += " | Avail: " + node.availableCount;
			}
			if (node.boundCount !== undefined && node.boundCount > 0) {
				countText += " | Bound: " + node.boundCount;
			}
			ctx.fillText(truncateText(ctx, countText, root.nodeWidth - 10), textX, textY);
			
			// Draw children
			if (layout.children) {
				for (let i = 0; i < layout.children.length; i++) {
					drawNodes(ctx, layout.children[i]);
				}
			}
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
