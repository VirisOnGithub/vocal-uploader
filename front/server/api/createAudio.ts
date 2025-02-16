export default defineEventHandler(async (event) => {
    const body = await readBody(event);
    const form = body.form;

    const createAudio = await $fetch('http://back:5000/files', {
        method: 'POST',
        headers: {
            'Content-Type': 'multipart/form-data',
            body: form
        },
    });

    return createAudio;
});