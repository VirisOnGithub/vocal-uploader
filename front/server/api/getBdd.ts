import { createConnection } from 'mysql';

export async function connectToDatabase() {
    const connection = createConnection({
        host: 'local_db',
        user: 'root',
        password: 'super_strong_password',
        database: 'vcupload'
    });
    return connection;
}

export default defineEventHandler(async () => {
    return connectToDatabase();
});