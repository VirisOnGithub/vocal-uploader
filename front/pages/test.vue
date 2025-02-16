<template>
    <p>{{ trackInfos }}</p>
</template>

<script setup>
    const trackInfos = await $fetch('/api/getFileByName', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ name: 'file_example_MP3_700KB.mp3' })
    })
</script>