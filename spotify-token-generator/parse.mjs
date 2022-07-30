import { resolve } from "path"
import { readFileSync } from "fs"


const content = readFileSync(resolve("./mct-list.json"), { encoding: "utf-8" })
const json = JSON.parse(content)

console.log(json.items)

console.log(json.items.map(({ track }) => ({ name: track.name, id: track.id })))
