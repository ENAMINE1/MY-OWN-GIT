# git_clone_completion.sh

_git_clone()
{
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts="init add commit clone hash-object write-tree ls-tree cat-file write-commit"

    if [[ ${cur} == -* ]]; then
        COMPREPLY=( $(compgen -W "--help -h" -- ${cur}) )
        return 0
    fi

    case "${prev}" in
        git_2.0)
            COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
            return 0
            ;;
        *)
            ;;
    esac

    COMPREPLY=( $(compgen -f -- ${cur}) )
    return 0
}
complete -F _git_clone git_2.0
