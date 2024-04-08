<script>
    import Dotbar from './Dotbar.svelte';
	import Header from './Header.svelte';
	import './styles.css';
	import TailwindCss from '../lib/TailwindCSS.svelte';

</script>

<TailwindCss />
<div class="flex flex-col min-h-screen">
    <main class="flex flex-col flex-grow">
        <Header />
        <!-- Slot container to extend down as far as possible -->
        <div class="flex flex-col flex-grow">
            <slot />
        </div>
    </main>
    <Dotbar />
</div>

<style>
	main {
		width: 100%;
		max-width: 64rem;
		margin: 0 auto;
		box-sizing: border-box;
	}
</style>
