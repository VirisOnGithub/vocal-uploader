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

onMounted(() => {
    if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
        alert('Your browser does not support the MediaRecorder API');
    }
})

let recordedChunks = [];

function startRecording(){
    navigator.mediaDevices.getUserMedia({ audio: true })
            .then(stream => {
                audio.value = document.querySelector('audio');
                mediaRecorder = new MediaRecorder(stream);
                mediaRecorder.ondataavailable = e => {
                    recordedChunks.push(e.data);
                };
                mediaRecorder.start();
            })
}

function stopRecording(){
    mediaRecorder.stop();
    mediaRecorder.onstop = () => {
        const audioBlob = new Blob(recordedChunks, { type: 'audio/mp3' });
        const audioUrl = URL.createObjectURL(audioBlob);
        audio.value.src = audioUrl;
        exportData();
        recordedChunks = [];
    }
}
function exportData(){
    const audioBlob = new Blob(recordedChunks, { type: 'audio/mp3' });
    const audioUrl = URL.createObjectURL(audioBlob);
    const a = document.createElement('a');
    a.href = audioUrl;
    a.download = 'audio.mp3';
    a.click();
}
</script>
