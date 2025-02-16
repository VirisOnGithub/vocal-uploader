import { connectToDatabase } from './getBdd';

export default defineEventHandler(async (event) => {
    const body = await readBody(event);
    const name = body.name;

    if (!name) {
        throw createError({ statusCode: 400, message: 'Missing name' });
    }

    try {
        const connection = await connectToDatabase();
        return new Promise((resolve, reject) => {
            connection.query('SELECT * from File', function (error, results) {
                if (error) {
                    reject(error);
                    return;
                }
                resolve(results);
            });
        });
    } catch (error) {
        throw createError({ statusCode: 500, message: 'Database query failed' });
    }
});