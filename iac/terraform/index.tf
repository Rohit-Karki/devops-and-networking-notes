provider "google" {
  project = "acme-app"
  region  = "us-central1"
}



data "aws_ami" "ubuntu" {
  most_recent = true

  filter {
    name   = "name"
    values = ["ubuntu/images/hvm-ssd/ubuntu-focal-20.04-amd64-server-*"]
  }

  filter {
    name   = "virtualization-type"
    values = ["hvm"]
  }

  owners = ["099720109477"] # Canonical
}


resource "aws_instance" "ec2web"{
    instance_type = "t2.micro"
    ami = data.aws_ami.ubuntu.id

    lifecycle{
        precondition{
            condition = data.aws_ami.ec2web.architecture == "x86_64"
            error_message = "The selected AMI must be for the x86_64 architecture."
        }
    }
}

resource "aws_db_instance" "example" {
  

  timeouts {
    create = "60m"
    delete = "2h"
  }
}