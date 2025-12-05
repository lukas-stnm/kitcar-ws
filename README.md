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
```
colcon build  
```
This will build the workspace and make you ready for development.

***

## **SSH Agent Forwarding**

Private repos require SSH keys. VS Code can forward your host SSH agent into the container.

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
