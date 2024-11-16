import { defineConfig } from 'astro/config';
import tailwind from '@astrojs/tailwind';
import vercelStatic from '@astrojs/vercel/static';

export default defineConfig({
  site: 'https://www.robrothschild.com',
  integrations: [tailwind()],
  // Vercel things
  output: 'static',
  adapter: vercelStatic({
    webAnalytics: {
      enabled: true,
    },
  }),
});