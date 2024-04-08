<script>
	import TailwindCss from '../lib/TailwindCSS.svelte';

	let mouseX = 0;
	let mouseY = 0;
	let showCursorCircle = false;
	let containerElement; // Reference to the container element
	function handleMousemove(event) {
		const rect = containerElement.getBoundingClientRect(); // Handle margins changing
		mouseX = event.clientX - rect.left;
		mouseY = event.clientY - rect.top;
		showCursorCircle = true;
	}
	function handleMouseLeave() {
        showCursorCircle = false;
		circles = circles.map(circle => ({
			...circle,
			offsetX: 0,
			offsetY: 0,
			color: 'rgb(0, 0, 0)', // Optional: Reset the color to the original (e.g., blue)
		}));
    }
	// Generate initial positions
  	const spacing = 10; // Spacing between circles
	const generateInitialPositions = () => {
		const circles = [];
		let numRows = 20;
		let numCols = 200;
		for (let i = 0; i < numRows * numCols; i++) {
			let row = Math.floor(i / numCols);
			let col = i % numCols;
			circles.push({
				id: i,
				x: col * spacing,
				y: row * spacing,
				offsetX: 0,
				offsetY: 0,
			});
		}
		return circles;
	};

	let circles = generateInitialPositions();

	// Calculate the distance between two points
	function distance(x1, y1, x2, y2) {
		return Math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2);
		}

	function calculateColor(displacement, maxDisplacement) {
		displacement = Math.min(displacement, maxDisplacement);
		const proportion = displacement / maxDisplacement;
		const grey = Math.round(255 * proportion);
		return `rgb(${grey}, ${grey}, ${grey})`;
		}

	let circleWidth = 10;
	let circleHeight = 10;
    const pushRadius = 40; // Radius within which circles will be pushed
	const maxDisplacement = 12;
	$: pushedCircles = circles.map(circle => {
		const circleCenterX = circle.x + circle.offsetX + circleWidth / 2;
		const circleCenterY = circle.y + circle.offsetY + circleHeight / 2;
		const dist = distance(circleCenterX, circleCenterY, mouseX, mouseY);
		if (dist < pushRadius) {
			const angle = Math.atan2(mouseY - circleCenterY, mouseX - circleCenterX);
			let displacement = dist;
			displacement = Math.min(displacement, maxDisplacement);
			return {
				...circle,
				offsetX: Math.cos(angle) * displacement, // Horizontal displacement
				offsetY: Math.sin(angle) * displacement, // Vertical displacement
				color: calculateColor(displacement, pushRadius),
			};
		}
		return { ...circle, color: 'rgb(150, 150, 150)' };
	});
</script>

<TailwindCss/>

<!-- svelte-ignore a11y-no-static-element-interactions -->
<div bind:this={containerElement} on:mousemove={handleMousemove} on:mouseleave={handleMouseLeave} 
	class="flex absolute inset-x-0 bottom-0 border border-red-500 h-64 z-0 overflow-hidden">
	{#if showCursorCircle}
		<div class="cursor-circle" style="left: {mouseX}px; top: {mouseY}px; width:{pushRadius}; hieght:{pushRadius}"></div>
	{/if}
	{#each pushedCircles as { id, x, y, offsetX, offsetY, color } (id)}
		<div
			class="circle"
			style="transform: translate({x + offsetX}px, {y + offsetY}px); background-color: {color};">
		</div>
	{/each}
</div>

<style>

    .circle {
        position: absolute;
        width: 10px;
        height: 10px;
		border-radius: 50%;
        transition: transform 1s;
    }

	.cursor-circle {
        position: absolute;
        border-radius: 50%;
        border: 2px solid red; /* Visible border without filling the circle */
        pointer-events: none; /* Prevents the circle from interfering with mouse events */
        transform: translate(-50%, -50%); /* Centers the circle around the cursor */
        transition: transform 20s ease-out 0.5s ease-in; /* Smooths out movement */
    }
</style>
