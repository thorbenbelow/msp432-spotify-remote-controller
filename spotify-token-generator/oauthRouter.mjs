import { Router } from "express"

const redirect_uri = "http://localhost:3000/"
const client_id = ""
const client_secret = ""

let access_token;
let refresh_token;

const router = new Router()

router.get("/stable", async (req, res) => {
  return res.send(`
  <input type="text" value="${access_token}" disabled>
  <input type="text" value="${refresh_token}" disabled>
  <form method="POST" action="/refresh"><button>Refresh</button></form>
`)
})

router.post("/refresh", async (req, res) => {
  await refreshAccessToken()
  return res.redirect("/stable")
})

router.get("/", async (req, res) => {
  if (req.query.code) {
    await fetchTokens(req.query.code)
    return res.redirect("/stable")
  } else {
    const url = requestAuthorization()
    return res.redirect(url)
  }
})

function requestAuthorization() {
  const url = new URL("https://accounts.spotify.com/authorize")
  url.searchParams.append("client_id", client_id)
  url.searchParams.append("response_type", "code")
  url.searchParams.append("redirect_uri", redirect_uri)
  url.searchParams.append("show_dialog", true)
  url.searchParams.append("scope", "user-read-private user-read-email user-modify-playback-state user-read-playback-position user-library-read streaming user-read-playback-state user-read-recently-played playlist-read-private")
  return url.toString()
}

async function refreshAccessToken() {
  const body = "grant_type=refresh_token"
    + "&refresh_token=" + refresh_token
    + "&client_id=" + client_id
  const tokens = await callAuthApi(body);
  console.log(tokens)
  access_token = tokens.access_token
}

async function callAuthApi(body) {
  const response = await fetch("https://accounts.spotify.com/api/token", {
    method: "POST",
    headers: {
      'Content-Type': "application/x-www-form-urlencoded",
      "Authorization": `Basic ${Buffer.from(`${client_id}:${client_secret}`, "binary").toString("base64")}`
    },
    body
  })
  return await response.json()

}

async function fetchTokens(code) {
  const body = "grant_type=authorization_code"
    + "&code=" + code
    + "&redirect_uri=" + encodeURI(redirect_uri)
    + "&client_id=" + client_id
    + "&client_secret=" + client_secret
  const tokens = await callAuthApi(body)
  access_token = tokens.access_token
  refresh_token = tokens.refresh_token
}

export default router;
