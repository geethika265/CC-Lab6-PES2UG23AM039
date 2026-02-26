pipeline {
    agent any

    stages {

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
                # Clean old containers
                docker rm -f nginx-lb backend1 backend2 || true

                # Clean + recreate network
                docker network rm app-network || true
                docker network create app-network

                # Start backends first
                docker run -d --name backend1 --network app-network backend-app
                docker run -d --name backend2 --network app-network backend-app

                # Give containers time to register DNS + start listening
                sleep 3
                '''
            }
        }

        stage('Deploy NGINX Load Balancer') {
            steps {
                sh '''
                # Run nginx with config mounted at startup (no docker cp)
                docker rm -f nginx-lb || true

                docker run -d \
                  --name nginx-lb \
                  --network app-network \
                  -p 80:80 \
                  -v "$PWD/nginx/default.conf:/etc/nginx/conf.d/default.conf:ro" \
                  nginx

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
