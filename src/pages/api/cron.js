
export default async function handler(req, res) {
    try {
      const response = await fetch('https://api.vercel.com/v1/integrations/deploy/prj_home-base', {
        method: 'POST',
        headers: {
          Authorization: `Bearer ${process.env.VERCEL_API_TOKEN}`,
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          target: 'production',
        }),
      });
  
      if (!response.ok) {
        const error = await response.json();
        return res.status(response.status).json({ error });
      }
  
      res.status(200).json({ message: 'Rebuild triggered successfully!' });
    } catch (error) {
      res.status(500).json({ error: error.message });
    }
  }
  