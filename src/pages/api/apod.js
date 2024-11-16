export async function get() {
    const NASA_API_KEY = import.meta.env.NASA_API_KEY;
    const APOD_URL = `https://api.nasa.gov/planetary/apod?api_key=${NASA_API_KEY}`;
  
    const response = await fetch(APOD_URL);
    const data = await response.json();
  
    if (!response.ok) {
      return new Response(
        JSON.stringify({ error: data.error?.message || response.statusText }),
        { status: response.status }
      );
    }
  
    return new Response(JSON.stringify(data), {
      headers: { 'Content-Type': 'application/json' },
    });
  }
  