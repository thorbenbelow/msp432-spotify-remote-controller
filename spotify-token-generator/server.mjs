import express from "express"
import bodyParser from "body-parser"
import oauthRouter from "./oauthRouter.mjs"
const app = express()
app.set("port", process.env.PORT || 3000)

app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: false }))

//app.get("/", (req, res) => void res.sendFile(resolve("./", "public", "index.html")))
app.use("/", oauthRouter)

app.listen(app.get("port"), () => {
  console.log(
    "  App is running at http://localhost:%d in %s mode",
    app.get("port"),
    app.get("env")
  );
  console.log("  Press CTRL-C to stop\n");
})

