from setuptools import find_packages
from setuptools import setup

setup(
    name='next_motor_interface',
    version='0.0.0',
    packages=find_packages(
        include=('next_motor_interface', 'next_motor_interface.*')),
)
