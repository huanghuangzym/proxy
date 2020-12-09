library "alauda-cicd@fix/adjust-docker-build"

echo "${BUILD_TAG} this is the tag"

def podYaml = """apiVersion: "v1"
kind: "Pod"
metadata:
  labels:
    jenkins: "slave"
    jenkins/label: "golang-kind"
spec:
  containers:
  - args:
    - "cat"
    command:
    - "/bin/sh"
    - "-c"
    env:
    - name: "no_proxy_1"
      value: "jenkins.default,jenkins-agent.default,kubernetes.default,10.96.0.1,athens.alauda.cn,https://athens.alauda.cn,https://athens.acp.alauda.cn,athens.acp.alauda.cn,googleapis.com,gomod.alauda.cn,https://registry.yarnpkg.com"
    - name: "http_proxy_1"
      value: "http://124.156.187.224:30500"
    - name: "JENKINS_URL"
      value: "https://jenkins-b.alauda.cn"
    image: "harbor-b.alauda.cn/asm/envoy-build-ubuntu-x86:0.2"
    imagePullPolicy: "Always"
    name: "clang"
    resources:
      requests:
        memory: "4000Mi"
        cpu: "2"
    securityContext:
      privileged: true
    tty: true
    volumeMounts:
    - mountPath: "/lib/modules"
      name: "volume-0"
      readOnly: false
    - mountPath: "/var/host/docker.sock"
      name: "volume-1"
      readOnly: false
    - mountPath: "/var/lib/docker"
      name: "volume-2"
      readOnly: false
    - mountPath: "/root/.cache"
      name: "volume-3"
      readOnly: false
    - mountPath: "/sys/fs/cgroup"
      name: "volume-4"
      readOnly: false 
    - mountPath: "/var/run"
      name: "volume-5"
      readOnly: false
    - mountPath: "/tmp"
      name: "volume-6"
      readOnly: false
    - mountPath: "/home/jenkins/agent"
      name: "workspace-volume"
      readOnly: false
    workingDir: "/home/jenkins/agent"
  - env:
    - name: "JENKINS_TUNNEL"
      value: "jenkins-agent.default:50000"
    - name: "GIT_SSL_NO_VERIFY"
      value: "1"
    - name: "no_proxy_1"
      value: "jenkins.default,jenkins-agent.default,kubernetes.default,10.96.0.1,athens.alauda.cn,https://athens.alauda.cn,https://athens.acp.alauda.cn,athens.acp.alauda.cn,googleapis.com,gomod.alauda.cn,https://registry.yarnpkg.com"
    - name: "http_proxy_1"
      value: "http://124.156.187.224:30500"
    - name: "JENKINS_AGENT_WORKDIR"
      value: "/home/jenkins/agent"
    - name: "JENKINS_URL"
      value: "https://jenkins-b.alauda.cn"
    image: "10.0.128.20:60080/devops/jnlp-slave:4.0.1-1-jdk11-v3.0.14.15"
    imagePullPolicy: "IfNotPresent"
    name: "jnlp"
    resources:
      limits: {}
      requests: {}
    securityContext:
      privileged: false
    tty: false
    volumeMounts:
    - mountPath: "/lib/modules"
      name: "volume-0"
      readOnly: false
    - mountPath: "/var/host/docker.sock"
      name: "volume-1"
      readOnly: false
    - mountPath: "/var/lib/docker"
      name: "volume-2"
      readOnly: false
    - mountPath: "/go"
      name: "volume-3"
      readOnly: false
    - mountPath: "/sys/fs/cgroup"
      name: "volume-4"
      readOnly: false
    - mountPath: "/var/run"
      name: "volume-5"
      readOnly: false
    - mountPath: "/tmp"
      name: "volume-6"
      readOnly: false
    - mountPath: "/home/jenkins/agent"
      name: "workspace-volume"
      readOnly: false
    workingDir: "/home/jenkins/agent"
  - args:
    - "cat"
    command:
    - "/bin/sh"
    - "-c"
    env:
    - name: "GIT_SSL_NO_VERIFY"
      value: "1"
    - name: "no_proxy_1"
      value: "jenkins.default,jenkins-agent.default,kubernetes.default,10.96.0.1,athens.alauda.cn,https://athens.alauda.cn,https://athens.acp.alauda.cn,athens.acp.alauda.cn,googleapis.com,gomod.alauda.cn,https://registry.yarnpkg.com"
    - name: "http_proxy_1"
      value: "http://124.156.187.224:30500"
    - name: "DOCKER_HOST"
      value: "unix:///var/host/docker.sock"
    - name: "JENKINS_URL"
      value: "https://jenkins-b.alauda.cn"
    image: "10.0.128.20:60080/devops/builder-tools:ubuntu-v3.0.0"
    imagePullPolicy: "IfNotPresent"
    name: "tools"
    resources:
      limits: {}
      requests: {}
    securityContext:
      privileged: false
    tty: true
    volumeMounts:
    - mountPath: "/lib/modules"
      name: "volume-0"
      readOnly: false
    - mountPath: "/var/host/docker.sock"
      name: "volume-1"
      readOnly: false
    - mountPath: "/var/lib/docker"
      name: "volume-2"
      readOnly: false
    - mountPath: "/root/.cache"
      name: "volume-3"
      readOnly: false
    - mountPath: "/sys/fs/cgroup"
      name: "volume-4"
      readOnly: false
    - mountPath: "/var/run"
      name: "volume-5"
      readOnly: false
    - mountPath: "/tmp"
      name: "volume-6"
      readOnly: false
    - mountPath: "/home/jenkins/agent"
      name: "workspace-volume"
      readOnly: false
    - mountPath: /root/.docker
      name: docker-config
      readOnly: false
    workingDir: "/home/jenkins/agent"
  - env:
    - name: "DOCKER_TLS_CERTDIR"
      value: ""
    - name: "no_proxy_1"
      value: "jenkins.default,jenkins-agent.default,kubernetes.default,10.96.0.1,athens.alauda.cn,https://athens.alauda.cn,https://athens.acp.alauda.cn,athens.acp.alauda.cn,googleapis.com,gomod.alauda.cn,https://registry.yarnpkg.com"
    - name: "http_proxy_1"
      value: "http://124.156.187.224:30500"
    - name: "JENKINS_URL"
      value: "https://jenkins-b.alauda.cn"
    image: "harbor-b.alauda.cn/devops/docker:19-dind"
    imagePullPolicy: "IfNotPresent"
    name: "docker"
    resources:
      limits:
        cpu: "2"
        ephemeral-storage: "10Gi"
      requests:
        memory: "512M"
        cpu: "1"
        ephemeral-storage: "10Gi"
    securityContext:
      privileged: true
    tty: false
    volumeMounts:
    - mountPath: "/lib/modules"
      name: "volume-0"
      readOnly: false
    - mountPath: "/var/host/docker.sock"
      name: "volume-1"
      readOnly: false
    - mountPath: "/var/lib/docker"
      name: "volume-2"
      readOnly: false
    - mountPath: "/go"
      name: "volume-3"
      readOnly: false
    - mountPath: "/sys/fs/cgroup"
      name: "volume-4"
      readOnly: false
    - mountPath: "/var/run"
      name: "volume-5"
      readOnly: false
    - mountPath: "/tmp"
      name: "volume-6"
      readOnly: false
    - mountPath: "/home/jenkins/agent"
      name: "workspace-volume"
      readOnly: false
    workingDir: "/home/jenkins/agent"
  hostNetwork: false
  nodeSelector:
    beta.kubernetes.io/os: "linux"
    harbor: "build"
  restartPolicy: "Never"
  securityContext: {}
  serviceAccount: "jenkins"
  volumes:
  - hostPath:
      path: "/lib/modules"
    name: "volume-0"
  - hostPath:
      path: "/tmp/kindstorage/${BUILD_TAG}"
    name: "volume-2"
  - hostPath:
      path: "/var/run/docker.sock"
    name: "volume-1"
  - emptyDir:
      medium: ""
    name: "workspace-volume"
  - hostPath:
      path: "/sys/fs/cgroup"
    name: "volume-4"
  - name: "volume-3"
    persistentVolumeClaim:
      claimName: "asmamd64"
      readOnly: false
  - emptyDir:
      medium: ""
    name: "volume-6"
  - emptyDir:
      medium: ""
    name: "volume-5"
  - hostPath:
      path: /root/.docker
      type: ""
    name: docker-config
"""

echo podYaml

AlaudaPipeline {
    config = [
        pod: [
            yaml: podYaml,
        ],
        folder: '.',
        docker: [
            credentials: "alaudak8s",
            enabled: false
        ],
        sonar: [
            binding: "sonarqube",
            enabled: false
        ],
		sec: [
           enabled: false,
           block: false,
           scanMod: 1,
           custermOpts: '',
        ],

    ]
    env = [
        GO111MODULE: "on",
        GOPROXY: "https://athens.alauda.cn",
		CGO_ENABLED: "0",
		GOOS: "linux",
    ]
    yaml = "alauda.yaml"
    lab = [ABORT_PREVIOUS: true]
}

