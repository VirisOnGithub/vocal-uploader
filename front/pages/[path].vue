<template>
    <audio controls :src="`http://localhost:5000/filesraw/${route.params.path}.webm`"></audio>
</template>

<script setup>
const route = useRoute();
</script>