terraform {
  cloud {
    organization = "organization-name"
    workspaces {
      name = "learn-tfc-aws"
    }
  }
  # required_version = ">= 0.12"
  required_providers {
    aws = {
        source = "hashicorp/aws"
        version = '->= 3.27'
    }
  }
}

provider "aws" {
    profile = "default"
    region = "us-east-2"
}

# Prefix of the type aws_instance matches the provider aws
resource "aws_instance" "app_server" {
    ami = "ami-830c94e3"
    instance_type = "t2.micro"
    tags = {
        Name = var.instance_name
    }
}
# terraform apply -var "instance_name=YetAnotherName"