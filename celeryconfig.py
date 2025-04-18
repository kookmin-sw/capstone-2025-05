from kombu import Exchange, Queue

broker_url = "redis://redis:6379/0"
result_backend = "redis://redis:6379/1"

task_queues = (
    Queue('default', Exchange('default'), routing_key='default'),
    Queue('audio_analysis', Exchange('audio_analysis'), routing_key='audio_analysis'),
)

task_default_queue = 'default'
task_default_exchange = 'default'
task_default_routing_key = 'default'

task_routes = {
    'workers.tasks.analyze_audio': {'queue': 'audio_analysis'},
    'workers.tasks.compare_audio': {'queue': 'audio_analysis'},
}

# Concurrency settings
worker_concurrency = 2

# Task execution settings
task_acks_late = True
task_time_limit = 600  # 10 minutes (for long-running audio processing)
task_soft_time_limit = 500  # 8.3 minutes

# Task result settings
task_ignore_result = False
task_store_errors_even_if_ignored = True

# Serialization
task_serializer = 'json'
result_serializer = 'json'
accept_content = ['json']