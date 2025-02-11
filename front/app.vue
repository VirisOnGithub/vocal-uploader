<template>
    <div class="text-center flex flex-col">
        <h1 class="text-3xl mt-3">Vocal Uploader</h1>
        <audio controls ref="audio" class="absolute left-1/2 bottom-0 pb-5 -translate-x-1/2 -translate-y-1/2"></audio>
        <button @click="toggleRecording" class="m-5">
             <UIcon v-if="recording" :class="iconStyle" name="i-material-symbols-stop" />
             <UIcon v-else :class="iconStyle" name="i-material-symbols-play-arrow" />
        </button>
        <div>
          <button @click="exportData"
                  class="bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded mt-5"
          >
            {{ $t('exportFile') }}
          </button>
        </div>
      <div>
        <button @click="setLocale('en')">English</button>
        <button @click="setLocale('fr')">Français</button>
      </div>
    </div>
</template>

<script setup lang="ts">
const { setLocale } = useI18n();
const iconStyle = 'h-10 w-10';
const audio = ref<HTMLAudioElement|null>(null);
const recording = ref<boolean>(false);
let mediaRecorder : MediaRecorder | null = null;
let recordedChunks : Blob[] = [];

defineShortcuts({
  'r': toggleRecording, // targets the space bar (as strange as it seems???)
  ' ': {
    handler: () => {
      audio.value?.paused ? audio.value?.play() : audio.value?.pause();
    }
  }
})

onMounted(() => {
    if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
        alert('Your browser does not support the MediaRecorder API');
    }
});

function toggleRecording(){
    if (recording.value) {
        stopRecording();
    } else {
        startRecording();
    }
    recording.value = !recording.value;
}

function startRecording() {
    recordedChunks = [];
    navigator.mediaDevices.getUserMedia({ audio: true })
        .then(stream => {
            audio.value = document.querySelector('audio');
            
            const options = { mimeType: 'audio/webm;codecs=opus' };
            
            if (!MediaRecorder.isTypeSupported(options.mimeType)) {
                console.warn('audio/webm not supported, using default format');
                mediaRecorder = new MediaRecorder(stream);
            } else {
                mediaRecorder = new MediaRecorder(stream, options);
            }

            mediaRecorder.ondataavailable = e => recordedChunks.push(e.data);
            mediaRecorder.start();
        })
        .catch(error => console.error("Error accessing microphone:", error));
}

function stopRecording() {
    if (!mediaRecorder) return;
    mediaRecorder.stop();
    mediaRecorder.onstop = () => {
        const audioBlob = new Blob(recordedChunks, { type: 'audio/webm' });
        const audioUrl = URL.createObjectURL(audioBlob);
        audio.value.src = audioUrl;
        setTimeout(() => URL.revokeObjectURL(audioUrl), 1000);
    };
}

function exportData() {
    if (recordedChunks.length === 0) return;
    const audioBlob = new Blob(recordedChunks, { type: 'audio/webm' });
    const audioUrl = URL.createObjectURL(audioBlob);
    const a = document.createElement('a');
    a.href = audioUrl;
    a.download = 'audio.webm';
    a.click();
    URL.revokeObjectURL(audioUrl);
}
</script>

<style>
* {
    font-family: 'Arial', sans-serif;
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}
</style>
