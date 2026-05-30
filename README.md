# 👋 Welcome to My Astro Portfolio

Welcome! This is the codebase for my personal portfolio website, built with [Astro](https://astro.build). It showcases my projects, experience, and interests in a fast, modern, and content-focused way.

The published project browsing experience now lives on the home page. Projects only open a new page when an `external_url` is present in [src/data/projects.ts](src/data/projects.ts).

---

## 🚀 Tech Stack

- [Astro](https://astro.build/) – Static site framework
- TypeScript / JavaScript
- CSS / Tailwind (optional, depending on setup)
- Markdown / MDX for content (if configured)

## 🧞 Astro Commands

If you want to use it as a template for your own site or just poke around in it, follow these instructions.

Clone the repository and install dependencies:

In a terminal:

git clone https://github.com/rarothschild/home_base.git
cd home_base

All commands are run from the root of the project, from a terminal:

| Command                   | Action                                           |
| :------------------------ | :----------------------------------------------- |
| `npm install`             | Installs dependencies                            |
| `npm run dev`             | Starts local dev server at `localhost:4321`      |
| `npm run build`           | Build your production site to `./dist/`          |
| `npm run preview`         | Preview your build locally, before deploying     |
| `npm run astro ...`       | Run CLI commands like `astro add`, `astro check` |
| `npm run astro -- --help` | Get help using the Astro CLI                     |

## Deployment Notes

Vercel should build this site from source using `npm run build`.

Do not commit `.vercel/output` or other generated deployment artifacts. If those files are checked in and Vercel is configured to deploy a static output directory, it can serve stale HTML/CSS/JS even when the source files under `src/` have changed.
