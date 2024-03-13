# The Resource Behavior page describes the general lifecycle for resources. Some details of that behavior can be customized using the special nested lifecycle block within a resource block body:
resource "azure_resource_group" "example"{
    lifecycle{
        create_and_destroy = true
    }
}

resource "aws_appautoscaling_target" "ecs_target" {
  lifecycle {
    replace_triggered_by = [
      # Replace `aws_appautoscaling_target` each time this instance of
      # the `aws_ecs_service` is replaced.
      aws_ecs_service.svc.id
    ]
  }
}