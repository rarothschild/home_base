export interface Project {
	slug: string;
	title: string;
	mini: string;
	status: string;
	external_url?: string;
	thumbnail?: string;
	imageAlt: string;
	summary: string;
	stack: string[];
	cta?: string;
	category: string;
	featured: boolean;
}

export const projects: Project[] = [
	{
		slug: 'bazaar',
		title: 'Bazaar Market',
		mini: 'Marketplace platform',
		status: 'Full-stack commerce',
		external_url: 'https://bazaar.rocks',
		thumbnail: '/images/bazaar/thumbnail.png',
		imageAlt: 'Bazaar marketplace storefront with product cards, search filters, and a map view.',
		summary: 'A full-stack marketplace for used gear, vehicles, tools, and other high-value goods.',
		stack: ['SvelteKit', 'Django', 'PostgreSQL', 'Tailwind CSS'],
		cta: 'Visit bazaar.rocks',
		category: 'Web Dev',
		featured: true,
	},
	{
		slug: 'test_interface',
		title: 'Testing Dashboard',
		mini: 'Hardware test GUI',
		status: 'Lab workflow tooling',
		thumbnail: '/images/testInspector/thumbnail.png',
		imageAlt: 'MATLAB testing interface for comparing analyses, models, and measured data.',
		summary: 'A custom dashboard for comparing collected test data against model predictions and engineering expectations.',
		stack: ['MATLAB', 'Python', 'SQL', 'Signal Processing'],
		category: 'Engineering',
		featured: true,
	},
	{
		slug: 'base_defender',
		title: 'Base Defender',
		mini: 'Physics + control gameplay',
		status: 'C++ prototype',
		thumbnail: '/images/lunar_defender/thumbnail.png',
		imageAlt: 'Base Defender gameplay showing a ship, altitude indicators, and obstacles.',
		summary: 'An arcade-style C++ project focused on moment-to-moment control, collision, and responsive gameplay systems.',
		stack: ['C++', 'Emscripten', 'Gameplay Systems'],
		category: 'Interactive Systems',
		featured: true,
	},
	{
		slug: 'simulytics',
		title: 'Simulytics',
		mini: 'Modeling dashboard',
		status: 'Analytics platform',
		thumbnail: '/images/simulytics/thumbnail.png',
		imageAlt: 'Simulytics dashboard for models, analyses, and engineering plots.',
		summary: 'An interactive modeling and analysis dashboard built to make simulation outputs easier to inspect and compare.',
		stack: ['Python', 'Django', 'SvelteKit', 'Flask'],
		category: 'Engineering',
		featured: true,
	},
	{
		slug: 'londonderry_quarry',
		title: 'Londonderry Quarry',
		mini: 'Site + visualization work',
		status: 'Client website',
		thumbnail: '/images/londonderry_quarry/thumbnail.png',
		imageAlt: 'Londonderry Quarry website with stonework photography and service details.',
		summary: 'An early client-facing website built to present a Pacific Northwest sculptor and quarry operation online.',
		stack: ['Astro', 'Static Site Design', 'Content Structure'],
		category: 'Web Dev',
		featured: true,
	},
	{
		slug: 'stock_trading',
		title: 'Stock Trader',
		mini: 'Market analytics prototype',
		status: 'Archived prototype',
		imageAlt: 'Archived stock-trading analytics prototype with charting and indicator exploration.',
		summary: 'A lightweight prototype for exploring indicators, trend behavior, and data-first market dashboards.',
		stack: ['Python', 'Data Analysis', 'Visualization'],
		category: 'Data Tools',
		featured: false,
	},
];

export const featuredProjects = projects.filter((project) => project.featured);
