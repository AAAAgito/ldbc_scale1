cmd_/home/vagrant/source/program2/Module.symvers := sed 's/ko$$/o/' /home/vagrant/source/program2/modules.order | scripts/mod/modpost -m -a   -o /home/vagrant/source/program2/Module.symvers -e -i Module.symvers   -T -
