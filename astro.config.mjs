import { defineConfig } from 'astro/config';
import tailwind from '@astrojs/tailwind';
// import vercelStatic from '@astrojs/vercel/static';
import vercel from '@astrojs/vercel/serverless'; // Use the "serverless" option

export default defineConfig({
  site: 'https://www.robrothschild.com',
  integrations: [tailwind()],
  // Vercel things
  output: 'static',
  adapter: vercelStatic({
    output: 'server', // Enable SSR
    adapter: vercel(), // Use the Vercel adapter
    webAnalytics: {
      enabled: true,
    },
  }),
});