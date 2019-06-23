workflow "Workflow" {
  on = "push"
  resolves = ["Check"]
}

action "Build and Test" {
  uses = "docker://tee3/fedora-29-boost:0.0.1"
  runs = ["sh", "-c", "bjam"]
}

action "Install" {
  needs = "Build and Test"
  uses = "docker://tee3/fedora-29-boost:0.0.1"
  runs = ["sh", "-c", "bjam --prefix=${HOME}/tmp install"]
}

action "Check" {
  needs = "Install"
  uses = "docker://tee3/fedora-29-boost:0.0.1"
  runs = ["sh", "-c", "test -d ${HOME}/tmp/bin"]
}
