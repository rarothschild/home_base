<script>

	import Counter from './Counter.svelte';
	import welcome from '$lib/images/svelte-welcome.webp';
	import welcome_fallback from '$lib/images/svelte-welcome.png';

	// Step 1: Define state for cursor position
	let mouseX = 0;
	let mouseY = 0;
	let showCursorCircle = false;

	// Step 3: Function to update position on mousemove
	let containerElement; // Reference to the container element
	function handleMousemove(event) {
		const rect = containerElement.getBoundingClientRect();
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
			color: 'rgb(0, 0, 255)', // Optional: Reset the color to the original (e.g., blue)
		}));
    }

	// Generate initial positions
  	const spacing = 12; // Spacing between circles
	const generateInitialPositions = () => {
		const circles = [];
		let numRows = 50;
		let numCols = 50;
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
		// Ensure displacement is within bounds
		displacement = Math.min(displacement, maxDisplacement);
		
		// Calculate the proportion of the displacement
		const proportion = displacement / maxDisplacement;

		// Interpolate the color components from blue (0, 0, 255) to red (255, 0, 0)
		const red = Math.round(255 * proportion);
		const blue = Math.round(255 * (1 - proportion));

		return `rgb(${red}, 0, ${blue})`;
		}

	let circleWidth = 10;
	let circleHeight = 10;
	$: pushedCircles = circles.map(circle => {
		const circleCenterX = circle.x + circle.offsetX + circleWidth / 2;
		const circleCenterY = circle.y + circle.offsetY + circleHeight / 2;
		const dist = distance(circleCenterX, circleCenterY, mouseX, mouseY);
		const pushRadius = 50; // Radius within which circles will be pushed
		if (dist < pushRadius) {
			const angle = Math.atan2(mouseY - circleCenterY, mouseX - circleCenterX);
			const displacement = pushRadius - dist;
			return {
				...circle,
				offsetX: Math.cos(angle) * (pushRadius - dist), // Horizontal displacement
				offsetY: Math.sin(angle) * (pushRadius - dist), // Vertical displacement
				color: calculateColor(displacement, pushRadius),
			};
		}
		return { ...circle, color: 'rgb(0, 0, 255)' };
	});
</script>

<svelte:head>
	<title>Robs Site</title>
	<meta name="description" content="Rob's Portforlio" />
</svelte:head>

<!-- svelte-ignore a11y-no-static-element-interactions -->
<div bind:this={containerElement} on:mousemove={handleMousemove} on:mouseleave={handleMouseLeave} class="container">
	{#if showCursorCircle}
		<div class="cursor-circle" style="left: {mouseX}px; top: {mouseY}px;"></div>
	{/if}
    {#each pushedCircles as { id, x, y, offsetX, offsetY, color } (id)}
        <div
            class="circle"
            style="transform: translate({x + offsetX}px, {y + offsetY}px); background-color: {color};">
        </div>
    {/each}
</div>

<style>

    .container {
        position: relative;
        width: 100vw;
        height: 100vh;
    }

    .circle {
        position: absolute;
        width: 10px;
        height: 10px;
        border-radius: 50%;
        background-color: rgb(112, 188, 218);
        transition: transform 0.3s; /* Correct property for animation */
    }

	.cursor-circle {
        position: absolute;
        width: 50px; /* Example size, adjust based on your push radius */
        height: 50px; /* Same as width to make it circular */
        border-radius: 50%;
        border: 2px solid red; /* Visible border without filling the circle */
        pointer-events: none; /* Prevents the circle from interfering with mouse events */
        transform: translate(-50%, -50%); /* Centers the circle around the cursor */
        transition: transform 0.1s ease; /* Smooths out movement */
    }
</style>
