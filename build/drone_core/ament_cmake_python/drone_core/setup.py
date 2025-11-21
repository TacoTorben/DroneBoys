from setuptools import find_packages
from setuptools import setup

setup(
    name='drone_core',
    version='0.0.1',
    packages=find_packages(
        include=('drone_core', 'drone_core.*')),
)
