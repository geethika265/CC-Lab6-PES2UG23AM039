pipeline {
    agent any

    stages {
        stage('Checkout SCM') {
            steps {
                checkout scm
            }
        }

        stage('Build Backend Image') {
            steps {
                sh '''
                  set -e
                  docker rmi -f backend-app || true
                  docker build -t backend-app backend
                '''
            }
        }

        stage('Deploy Backend Containers') {
            steps {
                sh '''
                  set -e

                  # Clean old containers if any
                  docker rm -f nginx-lb backend1 backend2 || true

                  # Clean + recreate network
                  docker network rm app-network || true
                  docker network create app-network

                  # Run 2 backends with FIXED HOSTNAMES (so output shows backend1/backend2)
                  docker run -d --name backend1 --hostname backend1 --network app-network backend-app
                  docker run -d --name backend2 --hostname backend2 --network app-network backend-app

                  # Small wait so containers are ready
                  sleep 2
                '''
            }
        }

        stage('Deploy NGINX Load Balancer') {
            steps {
                sh '''
                  set -e
                  docker rm -f nginx-lb || true

                  # Start nginx
                  docker run -d --name nginx-lb --network app-network -p 80:80 nginx

                  # Copy config from workspace into container
                  docker cp nginx/default.conf nginx-lb:/etc/nginx/conf.d/default.conf

                  # Restart nginx so it loads the new config cleanly
                  docker restart nginx-lb

                  # Validate config
                  docker exec nginx-lb nginx -t
                '''
            }
        }
    }

    post {
        success {
            echo 'Pipeline executed successfully. NGINX load balancer is running.'
        }
        failure {
            echo 'Pipeline failed. Check console logs for errors.'
        }
    }
}
