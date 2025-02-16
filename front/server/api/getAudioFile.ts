export default defineEventHandler(async (event) => {
    const req = await $fetch('http://localhost:5000/filesraw/mol', {});
});