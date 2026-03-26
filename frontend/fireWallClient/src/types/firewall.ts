export type RuleLayer = 'L2' | 'L3' | 'L4Simple' | 'L4TCP'

export type BaseRule = {
  ID: number
  ruleType: RuleLayer
  save?: boolean
  update?: boolean
}

export type L2RuleData = {
  permit?: boolean
  sourceMAC?: string
  destMAC?: string
  limitCount?: number
}

export type L3RuleData = {
  permit?: boolean
  source?: [string, number]
  dest?: [string, number]
  protocol?: string
  ttlMin?: number
  ttlMax?: number
  tos?: number
  allowFrag?: boolean
  limitCount?: number
}

export type L4SimpleRuleData = {
  permit?: boolean
  sourcePort?: number
  destPort?: number
  limitCount?: number
}

export type L4TCPRuleData = {
  permit?: boolean
  sourcePort?: number
  destPort?: number
  tcpFlags?: number
  flags?: number
  minWindowsize?: number
  maxWindowsize?: number
  minWin?: number
  maxWin?: number
  limitCount?: number
}

export type L2Rule = BaseRule & {
  ruleType: 'L2'
  data?: L2RuleData
}

export type L3Rule = BaseRule & {
  ruleType: 'L3'
  data?: L3RuleData
}

export type L4SimpleRule = BaseRule & {
  ruleType: 'L4Simple'
  data?: L4SimpleRuleData
}

export type L4TCPRule = BaseRule & {
  ruleType: 'L4TCP'
  data?: L4TCPRuleData
}

export type FirewallRule = L2Rule | L3Rule | L4SimpleRule | L4TCPRule