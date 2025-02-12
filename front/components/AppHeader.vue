<template>
    <header class="flex justify-between items-center p-4 bg-gray-800 text-white">
        <div></div>
        <h1>Vocal Uploader</h1>
        <div>
            <button @click="setLocale('en')">English</button>
            <button @click="setLocale('fr')">Français</button>
        </div>
    </header>
</template>

<script setup lang="ts">
const { setLocale } = useI18n();
</script>