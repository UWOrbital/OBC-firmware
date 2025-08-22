// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

// https://astro.build/config
export default defineConfig({
  site: 'https://uworbital.github.io',
  base: '/docs',
	integrations: [
		starlight({
			title: 'UW Orbital',
			social: [{ icon: 'github', label: 'GitHub', href: 'https://github.com/UWOrbital/OBC-firmware' }],
			sidebar: [
				{
					label: 'Getting Started',
					autogenerate: { directory: 'getting-started' },
				},
				{
					label: 'Guides',
					items: [
						// Each item here is one entry in the navigation menu.
						{ label: 'Example Guide', slug: 'guides/example' },
					],
				},
			],
		}),
	],
});
