<template>
    <header class="flex justify-between items-center p-4 bg-gray-800 text-white">
        <div>
            <NuxtLink to="/mol" class="text-white">Mol</NuxtLink>
        </div>
        <h1 class="text-4xl font-bold"><NuxtLink to="/">Vocal Uploader</NuxtLink></h1>
        <div>
            <UDropdown :items="items">
                <UButton color="white" :label="$t('language')" trailing-icon="i-heroicons-chevron-down-20-solid"/>
            </UDropdown>
        </div>
    </header>
</template>

<script setup lang="ts">
const { setLocale, locale } = useI18n();

const items = [
    [{
        label: 'Français',
        avatar: {
            src: 'https://flagcdn.com/w80/fr.webp'
        },
        click: () => setLocale('fr')
    }], [{
        label: 'English',
        avatar: {
            src: 'https://flagcdn.com/w80/gb.webp'
        },
        click: () => setLocale('en')
    }]
]
</script>