// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

// https://astro.build/config
export default defineConfig({
  site: 'https://uworbital.github.io',
  base: '/OBC-firmware/',
	integrations: [
		starlight({
      customCss: [
        './src/styles/custom.css',
      ],
      expressiveCode: {
        themes: ['github-light-high-contrast', 'tokyo-night'],
        removeUnusedThemes: true,
        styleOverrides: {
          // You can also override styles
          borderRadius: '0.5rem',
          borderColor: ['gray', 'black'],
          frames: {
            terminalTitlebarBackground: ['#414868', 'lightGray'],
            terminalTitlebarBorderBottomColor: ['gray', 'black'],
            terminalTitlebarDotsOpacity: '0.5',
            terminalTitlebarForeground: ['white', 'black']
          }
        },
      },
			title: 'UW Orbital',
			social: [{ icon: 'github', label: 'GitHub', href: 'https://github.com/UWOrbital/OBC-firmware' }],
			sidebar: [
				{
					label: 'Getting Started',
					autogenerate: { directory: 'getting-started' },
				},
        {
          label: 'Build Guides',
          autogenerate: { directory: 'builds'},
        },
				{
					label: 'Comms Guides',
					autogenerate: { directory: 'comms' },
				},
				{
					label: 'Guides',
					autogenerate: { directory: 'guides' },
				},
			],
		}),
	],
});
