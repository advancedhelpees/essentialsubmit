# essentialsubmit

This is an application to submit essential.exefs from SD (or NAND) to a server. 

# Usage

* Application is available in cia and 3dsx format
    * 3dsx -> /3ds/
    * cia -> install with CIA installer like FBI
1. Launch the application
1. Press Y to enter a discord username
1. (OPTIONAL) Press X and D-Pad Down to enter a custom server
1. Touch the soap on the bottom screen

Done!

# Building

Requires:
- 3ds-curl and 3ds-dev via [devkitPro](https://devkitpro.org/wiki/devkitPro_pacman)
- [makerom](https://github.com/3DSGuy/Project_CTR/tree/master/makerom)
- [bannertool](https://github.com/Epicpkmn11/bannertool)

```
make SERVER_ADDRESS=https://server.xyz/submit
```

# Hosting

app.py is a super small flask app that listens for post request on /submit and saves essential as essential_\<discordname\>.exefs in the essentials folder. To manage the essentials remotely, use the admin panel accessible under /admin. Username/password are set in config.ini.

### Setup

1. Fill out config.ini.example and rename it to config.ini
2. Install gunicorn, flask and flask_httpauth via pip:
```bash
python3 -m venv .venv
. .venv/bin/activate
pip install -r requirements.txt
```
To run the flask app, activate the venv, open port 9023 in your firewall if needed, and run:
```bash
gunicorn -w 1 app:app -b 0.0.0.0:9023
```
