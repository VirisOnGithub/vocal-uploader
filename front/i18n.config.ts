export default defineI18nConfig(() => ({
    legacy: false,
    locale: 'en',
    messages: {
        en: {
            exportFile: 'Export File',
            language: 'Language',
            listening: 'Listening to audio',
        },
        fr: {
            exportFile: 'Exporter le fichier',
            language: 'Langue',
            listening: 'Écoute de l\'audio',
        }
    }
}))