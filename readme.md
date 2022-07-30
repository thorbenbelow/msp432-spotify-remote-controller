# msp432 Spotify remote controller

This project uses the msp432 to remote control spotify clients.

## Setup

### Fetch tokens
Create a spotify web app at https://developer.spotify.com/.
Set a redirect url and get client id and secret and add them to the oauth-app.
Run the app with `npm start` and add your token in tokens.h.

### Fetch certificates

```
cd ${0%/*} 2>/dev/null                                                                                                                                                        
python3 cert.py -s api.spotify.com -n spotify > certs.h 
```
