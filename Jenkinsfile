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
                docker rmi -f backend-app || true
                docker build -t backend-app backend
                '''
            }
        }

        stage('Deploy Backend Containers') {
            steps {
                sh '''
                # Remove old containers if any
                docker rm -f backend1 backend2 || true

                # Recreate network cleanly
                docker network rm app-network || true
                docker network create app-network

                # Start backend containers on the network
                docker run -d --name backend1 --network app-network backend-app
                docker run -d --name backend2 --network app-network backend-app
                '''
            }
        }

        stage('Deploy NGINX Load Balancer') {
            steps {
                sh '''
                # Remove old nginx if exists
                docker rm -f nginx-lb || true

                # Start nginx on same network
                docker run -d \
                  --name nginx-lb \
                  --network app-network \
                  -p 80:80 \
                  nginx

                # Copy updated nginx config
                docker cp nginx/default.conf nginx-lb:/etc/nginx/conf.d/default.conf

                # Test config inside container
                docker exec nginx-lb nginx -t

                # Restart to apply config
                docker restart nginx-lb
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
