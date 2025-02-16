<template>
    <UContainer>
        <UCard class="flex flex-col items-center  absolute left-1/2 top-1/2 -translate-x-1/2 -translate-y-1/2">
            <h1 class="text-2xl">{{ $t('listening') }} : "{{ route.params.path }}"</h1>
            <div @click="togglePlay" class="m-5 flex justify-center items-center">
                <UIcon v-if="!isPlaying" :class="iconStyle" name="i-material-symbols-play-arrow" />
                <UIcon v-else :class="iconStyle" name="i-ic-sharp-pause" />
            </div>
            <div class="flex flex-col items-center">
                <UProgress ref="progressRef" :value="progressCount" />
                <div>{{ timerRef }}</div>
            </div>
        </UCard>
    </UContainer>
    <audio controls ref="audioRef" :src="`http://localhost:5000/filesraw/${route.params.path}`" class="absolute left-1/2 bottom-0 pb-5 -translate-x-1/2 -translate-y-1/2"></audio>
</template>

<script setup>
const route = useRoute();
const audioRef = ref(null);
const progressRef = ref(null);
const progressCount = ref(0);
const isPlaying = ref(false);
const iconStyle = 'h-10 w-10';
const timerRef = ref("");

const togglePlay = () => {
    const audio = audioRef.value;
    if (audio) {
        if (audio.paused) {
            audio.play();
        } else {
            audio.pause();
        }
    }
};

const getTimer = () => {
    const audio = audioRef.value;
    if (audio) {
        return `${convertTime(audio.currentTime)} / ${convertTime(audio.duration)}`;
    }
    return '';
};

const convertTime = (time) => {
    const minutes = Math.floor(time / 60).toFixed(0).padStart(2, '0');
    const seconds = Math.floor(time % 60).toFixed(0).padStart(2, '0');
    return `${minutes}:${seconds}`;
};

const updateIsPlaying = () => {
    const audio = audioRef.value;
    if (audio) {
        isPlaying.value = !audio.paused;
    }
};

const updateProgressBar = () => {
    const audio = audioRef.value;
    if (audio) {
        progressCount.value = (audio.currentTime / audio.duration) * 100 || 0;
    }
};

const putAudioAtCursorPosition = (event) => {
    const audio = audioRef.value;
    const progress = progressRef.value;
    if (audio && progress) {
        const { left, width } = progress.getBoundingClientRect();
        const clickX = event.clientX - left;
        const percentage = clickX / width;
        audio.currentTime = audio.duration * percentage;
    }
};

onMounted(() => {
    const audio = audioRef.value;
    if (audio) {
        audio.addEventListener('timeupdate', updateProgressBar);
        audio.addEventListener('play', updateIsPlaying);
        audio.addEventListener('pause', updateIsPlaying);
        // The following lines are necessary to initialize the timers (idk why, but the duration won't casts otherwise)
        // audio.play();
        // audio.pause();
        audio.addEventListener('loadedmetadata', () => {
            timerRef.value = getTimer();
        });
    }
});

onBeforeUnmount(() => {
    const audio = audioRef.value;
    if (audio) {
        audio.removeEventListener('timeupdate', updateProgressBar);
        audio.removeEventListener('play', updateIsPlaying);
        audio.removeEventListener('pause', updateIsPlaying);
    }
});

defineShortcuts({
  ' ': togglePlay,
})

</script>
