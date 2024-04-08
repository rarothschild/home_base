<script>
    import Dotbar from './Dotbar.svelte';
	import Header from './Header.svelte';
	import './styles.css';
	import TailwindCss from '../lib/TailwindCSS.svelte';

</script>

<TailwindCss />
<main class="flex flex-col flex-grow justify-center z-10">
	<Header />
	<slot />
</main>
<Dotbar />

<style>
	main {
		width: 100%;
		max-width: 64rem;
		margin: 0 auto;
		box-sizing: border-box;
	}
</style>
