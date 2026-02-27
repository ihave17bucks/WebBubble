# WebBubble Website Deployment Guide 🌐

This guide shows you how to deploy the WebBubble marketing website.

## Quick Deploy Options

### Option 1: Vercel (Recommended - Easiest)

1. **Install Vercel CLI**
   ```bash
   npm install -g vercel
   ```

2. **Deploy**
   ```bash
   cd website
   vercel
   ```

3. **Follow prompts**
   - Link to your account
   - Choose project settings
   - Done! You'll get a URL like: `webbubble.vercel.app`

4. **Custom Domain** (Optional)
   ```bash
   vercel --prod
   # Then add custom domain in Vercel dashboard
   ```

**Pros:** Free, fast, automatic HTTPS, CDN
**Cons:** None really!

---

### Option 2: Netlify (Also Great)

1. **Install Netlify CLI**
   ```bash
   npm install -g netlify-cli
   ```

2. **Deploy**
   ```bash
   cd website
   netlify deploy
   ```

3. **Follow prompts**
   - Authenticate
   - Create site
   - Deploy to production: `netlify deploy --prod`

4. **Or Use Drag & Drop**
   - Go to https://app.netlify.com/drop
   - Drag the `website` folder
   - Done!

**Pros:** Free, simple, great UI
**Cons:** None!

---

### Option 3: GitHub Pages (Free Forever)

1. **Create Repository**
   ```bash
   cd website
   git init
   git add .
   git commit -m "Initial commit"
   ```

2. **Push to GitHub**
   ```bash
   git remote add origin https://github.com/yourusername/webbubble-website.git
   git push -u origin main
   ```

3. **Enable GitHub Pages**
   - Go to repo Settings
   - Pages section
   - Source: main branch, / (root)
   - Save

4. **Access at**
   ```
   https://yourusername.github.io/webbubble-website
   ```

5. **Custom Domain** (Optional)
   - Add CNAME file: `echo "webbubble.dev" > CNAME`
   - Configure DNS: `A` record to GitHub IPs
   - Wait for DNS propagation

**Pros:** Free, simple, GitHub integrated
**Cons:** Slower than Vercel/Netlify

---

### Option 4: Cloudflare Pages (Best Performance)

1. **Sign up at Cloudflare Pages**
   - https://pages.cloudflare.com

2. **Connect Git Repository**
   - Or upload directly

3. **Build Settings**
   - Build command: (none)
   - Publish directory: `/`

4. **Deploy**
   - Automatic on push
   - Global CDN
   - Free SSL

**Pros:** Fastest CDN, free, unlimited bandwidth
**Cons:** Requires Cloudflare account

---

### Option 5: WebBubble Server (Dogfooding!)

When WebBubble is ready, serve the website with WebBubble itself!

```bubble
route "/*" {
    file = files.read("./website/" + request.path)
    response file
}
```

Or simpler:
```bash
webbubble-server --static ./website --port 80
```

**Pros:** Uses your own creation!
**Cons:** Need to manage server

---

## Custom Domain Setup

### Buy Domain
- Namecheap, Google Domains, or Cloudflare

### DNS Configuration

**For Vercel:**
```
Type: CNAME
Name: www
Value: cname.vercel-dns.com

Type: A
Name: @
Value: 76.76.21.21
```

**For Netlify:**
```
Type: CNAME
Name: www
Value: your-site.netlify.app

Type: A
Name: @
Value: 75.2.60.5
```

**For GitHub Pages:**
```
Type: A
Name: @
Value: 185.199.108.153
       185.199.109.153
       185.199.110.153
       185.199.111.153

Type: CNAME
Name: www
Value: yourusername.github.io
```

---

## Recommended Approach

**For Launch:**
1. Use **Vercel** for instant deployment
2. Get `webbubble.vercel.app` working
3. Buy domain: `webbubble.dev`
4. Connect custom domain
5. Launch! 🚀

**For Production:**
1. Migrate to **Cloudflare Pages** for best CDN
2. Keep Vercel as backup/staging
3. Use WebBubble server for API

---

## Quick Deploy Commands

### Vercel
```bash
cd website
vercel --prod
```

### Netlify
```bash
cd website
netlify deploy --prod
```

### Cloudflare
```bash
cd website
wrangler pages publish .
```

### GitHub Pages
```bash
cd website
git add .
git commit -m "Update"
git push
```

---

## Post-Deployment Checklist

- [ ] Test all pages load
- [ ] Check mobile responsiveness
- [ ] Verify HTTPS works
- [ ] Test all links
- [ ] Add Google Analytics
- [ ] Set up error tracking (Sentry)
- [ ] Submit to Google Search Console
- [ ] Add sitemap.xml
- [ ] Configure robots.txt

---

## Analytics Setup

### Google Analytics
Add before `</head>`:
```html
<!-- Google Analytics -->
<script async src="https://www.googletagmanager.com/gtag/js?id=G-XXXXXXXXXX"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());
  gtag('config', 'G-XXXXXXXXXX');
</script>
```

### Plausible (Privacy-friendly alternative)
```html
<script defer data-domain="webbubble.dev" src="https://plausible.io/js/script.js"></script>
```

---

## Performance Tips

1. **Enable Gzip/Brotli** - Most hosts do this automatically
2. **CDN** - Vercel/Netlify/Cloudflare all have global CDNs
3. **Optimize images** - Already minimal!
4. **Inline CSS** - Already done!

---

## Monitoring

### Uptime Monitoring
- UptimeRobot (free)
- Better Uptime
- Pingdom

### Performance Monitoring
- Google PageSpeed Insights
- WebPageTest
- Lighthouse CI

---

## SEO Setup

### Add sitemap.xml
```xml
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
  <url>
    <loc>https://webbubble.dev/</loc>
    <priority>1.0</priority>
  </url>
  <url>
    <loc>https://webbubble.dev/examples.html</loc>
    <priority>0.8</priority>
  </url>
</urlset>
```

### Add robots.txt
```
User-agent: *
Allow: /

Sitemap: https://webbubble.dev/sitemap.xml
```

---

## Ready to Deploy!

**Recommended first deployment:**
```bash
# 1. Deploy to Vercel
cd website
vercel --prod

# 2. You'll get a URL instantly!
# 3. Share it everywhere! 🚀
```

---

Need help? Open an issue or ask in Discord!
