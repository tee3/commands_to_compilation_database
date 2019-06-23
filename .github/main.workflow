workflow "Workflow" {
  on = "push"
  resolves = ["Check"]
}

action "Build and Test" {
  uses = "tee3/github-actions/cplusplus@master"
  runs = ["sh", "-c", "bjam"]
}

action "Install" {
  needs = "Build and Test"
  uses = "tee3/github-actions/cplusplus@master"
  runs = ["sh", "-c", "bjam --prefix=${HOME}/tmp install"]
}

action "Check" {
  needs = "Install"
  uses = "tee3/github-actions/cplusplus@master"
  runs = ["sh", "-c", "test -d ${HOME}/tmp/bin"]
}
