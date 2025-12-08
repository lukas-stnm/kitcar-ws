# **kitcar-ws**

`kitcar-ws` is the **workspace repository** for our ROS projects.  
It provides:

*   A **Dev Container** with all required tools and dependencies for consistent development.
*   A **workspace configuration** that automatically clones all essential project repositories.

***

## **Getting Started**

Clone and open in VS Code:

```bash
git clone git@github.com:KITcar-Team/kitcar-ws.git
cd kitcar-ws
code . # Open the project in vscode
```

Then click **Reopen in Container** when prompted. The container will set up the environment and fetch the required repos.

Now, open a terminal in vscode.
Running the command `pwd` should print _kitcar_.
Make sure that you're terminal is located in `~/ws` and run the command
```bash
colcon build --symlink-install
```
This will build the workspace and make you ready for development.

***

## Adressing problems: **SSH**

If the devcontainer cannot be started, the reason is often because there are problems with ssh-authentication
and cloning the required repos during the devcontainer setup.

Because of the way vscode displays logs, it often doesn't even report the error messages to you.
If you cannot build the devcontainer and you don't know why, firts try these steps.

### Authenticity of host cannot be established
If you have never connected to `git.kitcar-team.de` via ssh, the devcontainer will not build.
To mitigate this, in a fresh windows-terminal, run:
`ssh -T git@git.kitcar-team.de`

You should see a message like:

    The authenticity of host 'git.kitcar-team.de (173.249.38.174)' can't be established.
    ED25519 key fingerprint is: SHA256:nsLkJgu6EPa54dYbLlbF+I00E50u3LC5b518TMXjMJM
    This key is not known by any other names.
    Are you sure you want to continue connecting (yes/no/[fingerprint])?
    
Answer `yes`!
Then, you should see a message like

    Welcome to GitLab, <username>!
    Hi <username>! You've successfully authenticated with GitHub.

If something fails, make sure that you have properly registered your ssh key on the GitLab.

In case it worked, try following the steps above again and see if the devcontainer builds this time.

### SSH agent forwarding
Sometimes, vscode can't forward the ssh agent from the windows into the devcontainer.
In this case, follow the steps below in order to enable it.

**Important:** Make sure the SSH key you add is the one **registered with GitHub and GitLab** for your account.

**Linux/macOS:**

```bash
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519
```

**Windows (PowerShell):**

```powershell
Start-Service ssh-agent
ssh-add ~\.ssh\id_ed25519
```

Once the agent is running, VS Code handles forwarding automatically.

### **Verify access**

Inside the container:

```bash
ssh -T git@git.kitcar-team.de    # GitLab
ssh -T git@github.com            # GitHub
```

You should see a success message like:

    Welcome to GitLab, <username>!
    Hi <username>! You've successfully authenticated with GitHub.
