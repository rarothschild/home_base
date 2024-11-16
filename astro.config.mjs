import { defineConfig } from 'astro/config';
import tailwind from '@astrojs/tailwind';
// import vercelStatic from '@astrojs/vercel/static';
import vercel from '@astrojs/vercel/serverless'; // Use the "serverless" option

export default defineConfig({
  site: 'https://www.robrothschild.com',
  integrations: [tailwind()],
  // Vercel things
  output: 'server',
  adapter: vercel({
  }),
});