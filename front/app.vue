<template>
    <div>
        <h1>Vocal Uploader</h1>
        <audio controls ref="audio"></audio>
        <button @click="startRecording">Start Recording</button>
        <button @click="stopRecording">Stop Recording</button>
        <button @click="exportData">Export Data</button>
    </div>
</template>

<script setup>
const audio = ref(null);
let mediaRecorder;
let recordedChunks = [];

onMounted(() => {
    if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
        alert('Your browser does not support the MediaRecorder API');
    }
});

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
